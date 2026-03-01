#include "octaveparamspart2.h"
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QDebug>

OctaveRunnerPart2::OctaveRunnerPart2(QObject *parent)
    : QObject(parent), proc(nullptr) {}

void OctaveRunnerPart2::run()
{
    runOctave(params);   // выполняем исходный код
}

QString OctaveRunnerPart2::findOctaveExecutable()
{
    QStringList possiblePaths;

    // Стандартные пути установки на Windows
    possiblePaths << "C:/Program Files/GNU Octave/Octave-*/mingw64/bin/octave.exe";
    possiblePaths << "C:/Program Files (x86)/GNU Octave/Octave-*/mingw64/bin/octave.exe";
    possiblePaths << "C:/Users/*/AppData/Local/Programs/GNU Octave/Octave-*/mingw64/bin/octave.exe";
    possiblePaths << "E:/Octave/Octave-*/mingw64/bin/octave.exe";

    // Поиск через where в командной строке
    QProcess which;
    which.start("where octave");
    if (which.waitForFinished(3000)) {
        QString output = which.readAllStandardOutput().trimmed();
        if (!output.isEmpty()) {
            QStringList lines = output.split("\n", Qt::SkipEmptyParts);
            for (const QString &line : lines) {
                QString path = line.trimmed();
                if (QFile::exists(path)) {
                    qDebug() << "Found Octave via 'where':" << path;
                    return QDir::toNativeSeparators(path);
                }
            }
        }
    }

    // Поиск в PATH
    QString pathEnv = qgetenv("PATH");
    QStringList paths = pathEnv.split(";", Qt::SkipEmptyParts);
    for (const QString &path : paths) {
        QDir dir(path);
        QString exePath = dir.filePath("octave.exe");
        if (QFile::exists(exePath)) {
            qDebug() << "Found Octave in PATH:" << exePath;
            return QDir::toNativeSeparators(exePath);
        }
    }

    // Рекурсивный поиск по маске
    for (const QString &pattern : possiblePaths) {
        // Разбираем путь на части
        QString pathPattern = QDir::fromNativeSeparators(pattern);
        QStringList parts = pathPattern.split('/');

        // Начинаем поиск с корневого каталога
        QString currentPath = parts.first();
        if (currentPath.isEmpty()) currentPath = "/";

        // Рекурсивно ищем файл
        QString foundPath = findFileRecursive(currentPath, parts.mid(1));
        if (!foundPath.isEmpty()) {
            qDebug() << "Found Octave via pattern search:" << foundPath;
            return QDir::toNativeSeparators(foundPath);
        }
    }

// Поиск во всех дисках Windows
#ifdef Q_OS_WIN
    QFileInfoList drives = QDir::drives();
    for (const QFileInfo &drive : drives) {
        QString searchPath = drive.absolutePath() + "GNU Octave";
        QString foundPath = findFileRecursive(searchPath,
                                              {"Octave-*", "mingw64", "bin", "octave.exe"});
        if (!foundPath.isEmpty()) {
            qDebug() << "Found Octave on drive:" << foundPath;
            return QDir::toNativeSeparators(foundPath);
        }
    }
#endif

    qDebug() << "Octave executable not found";
    return QString();
}

// Вспомогательная рекурсивная функция для поиска по маске
QString OctaveRunnerPart2::findFileRecursive(const QString &startPath, const QStringList &patternParts, int depth)
{
    if (depth >= patternParts.size()) return QString();

    QDir dir(startPath);
    QString currentPattern = patternParts[depth];

    if (depth == patternParts.size() - 1) {
        // Последний элемент - ищем файл
        if (dir.exists(currentPattern)) {
            return dir.filePath(currentPattern);
        }
        return QString();
    }

    // Ищем подкаталоги по маске
    QStringList filters;
    filters << currentPattern;
    dir.setFilter(QDir::Dirs | QDir::NoDotAndDotDot);
    dir.setNameFilters(filters);

    QFileInfoList subDirs = dir.entryInfoList();
    for (const QFileInfo &subDir : subDirs) {
        QString result = findFileRecursive(
            subDir.absoluteFilePath(),
            patternParts,
            depth + 1
            );
        if (!result.isEmpty()) {
            return result;
        }
    }

    return QString();
}

void OctaveRunnerPart2::runOctave(OctaveParams_ &params)
{
    qDebug() << "[runOctave] Created in thread:" << QThread::currentThread();
    outDir = QDir::toNativeSeparators(QDir::currentPath() + "/results/");
    QDir().mkpath(outDir);
    qDebug() << "Results folder:" << outDir;

    QString scriptPath = QDir::toNativeSeparators(QDir::currentPath() + "/bch_lab_auto.m");
    writeOctaveScript(params, scriptPath, outDir);

    QString octaveProgram = findOctaveExecutable();

    if (octaveProgram.isEmpty()) {
        emit errorOccurred("❌ Octave executable not found. Please install GNU Octave or specify path manually.");
        return;
    }

    qDebug() << "Using Octave from:" << octaveProgram;

    QStringList args;
    args << "--no-gui" << "--silent" << scriptPath;

    proc = new QProcess(this);
    connect(proc, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this, [this](int exitCode, QProcess::ExitStatus){
                if(exitCode == 0) emit finished();
                else emit errorOccurred("Octave finished with error");
                proc->deleteLater(); proc = nullptr;
            });
    connect(proc, &QProcess::errorOccurred, this, [this](QProcess::ProcessError){
        emit errorOccurred("Octave launch error");
        if(proc) { proc->deleteLater(); proc=nullptr; }
    });
    connect(proc, &QProcess::readyReadStandardOutput, [this](){
        QByteArray b = proc->readAllStandardOutput();
        if(!b.isEmpty()) emit logMessage(QString::fromUtf8(b));
    });
    connect(proc, &QProcess::readyReadStandardError, [this](){
        QByteArray b = proc->readAllStandardError();
        if(!b.isEmpty()) emit logMessage(QString::fromUtf8(b));
    });

    proc->start(octaveProgram, args);
    if(!proc->waitForStarted(3000)) {
        emit errorOccurred("🆘 Не удалось подключится к Octave");
    } else {
        emit logMessage("⚙ Octave в процессе выполнения...");
    }
}

void OctaveRunnerPart2::writeOctaveScript(OctaveParams_ &p, const QString &path, const QString &outDirLocal)
{

    QFile f(path);
    if(!f.open(QIODevice::WriteOnly | QIODevice::Text)) return;
    QTextStream ts(&f);

    QString script = QString(R"(
close all;
clear;
pkg load signal;
pkg load communications;

n = %1;
k = %2;
t = %3;
num_words_default = %4;
channel_error_probability = %5;

out_dir = '%6';
if ~exist(out_dir,'dir'), mkdir(out_dir); end

function part2(random_sequence, n, k, t, channel_error_probability, out_dir)
    encoded_sequence = [];
    for i = 1:k:length(random_sequence)
        current_chunk = random_sequence(i:i+k-1);
        encoded_msg = bchenco(current_chunk, n, k);
        encoded_sequence = [encoded_sequence encoded_msg];
    end

    % Выход кодера
    figure; stem(encoded_sequence, 'filled'); title('Последовательность на выходе кодера'); xlabel('Номер'); ylabel('Значение'); grid on;

    received_sequence = encoded_sequence;
    error_vector = rand(1, length(received_sequence)) < channel_error_probability;
    received_sequence(error_vector) = ~received_sequence(error_vector);

    error_positions = abs(encoded_sequence - received_sequence);
    num_errors = sum(error_positions ~= 0);
    sequence_length = length(received_sequence);

    % Вход декодера
    figure; stem(received_sequence, 'filled'); title('Последовательность на входе декодера'); xlabel('Номер'); ylabel('Значение'); grid on;

    % Вектор ошибок
    figure; stem(error_positions, 'filled'); title('Вектор ошибок'); xlabel('Номер'); ylabel('Ошибка (0 или 1)'); grid on;

    fprintf('QQQ: %d\n', num_errors);

    % Сохраняем CSV для Qt
csvwrite(fullfile(out_dir, 'p3_encoded.csv'), encoded_sequence);
csvwrite(fullfile(out_dir, 'p3_received.csv'), received_sequence);
csvwrite(fullfile(out_dir, 'p3_error_vector.csv'), error_positions);

endfunction

random_sequence = round(rand(1, k * num_words_default));
part2(random_sequence, n, k, t, channel_error_probability, out_dir);
)").arg(p.n).arg(p.k).arg(p.t).arg(p.numWords).arg(p.channelErrorProbability).arg(outDirLocal);

    ts << script;
    f.close();
}
