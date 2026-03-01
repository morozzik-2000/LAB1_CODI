#include "OctaveRunner.h"
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include "OctaveParams.h"
#include <QThread>

OctaveRunner::OctaveRunner(QObject *parent)
    : QObject(parent), proc(nullptr)
{}


void OctaveRunner::run()
{
    runOctave(params);   // выполняем исходный код
}

QString OctaveRunner::findOctaveExecutable()
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
QString OctaveRunner::findFileRecursive(const QString &startPath, const QStringList &patternParts, int depth)
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

void OctaveRunner::runOctave(OctaveParams_ &params)
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

void OctaveRunner::writeOctaveScript(OctaveParams_ &p, const QString &path, const QString &outDirLocal)
{
    QFile f(path);
    if (!f.open(QIODevice::WriteOnly | QIODevice::Text))
        return;

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

function save_chart_csv_png(filename, data, out_dir)
    csvwrite(fullfile(out_dir, [filename '.csv']), data(:)');
    figure; stem(data, 'filled');
    saveas(gcf, fullfile(out_dir, [filename '.png']));
    close(gcf);
endfunction

function run_bch_model(random_sequence, n, k, t, out_dir)
    encoded_sequence = [];
    decoded_sequence = [];

    for i = 1:k:length(random_sequence)
        current_chunk = random_sequence(i:i+k-1);
        encoded_msg = bchenco(current_chunk, n, k);
        encoded_sequence = [encoded_sequence encoded_msg];

        received_msg = encoded_msg;
        decoded_msg = bchdeco(received_msg, n, t);
        decoded_msg = decoded_msg(end-k+1:end);
        decoded_sequence = [decoded_sequence decoded_msg];
    end

    % --- Построение трёх графиков ---
    figure;
    subplot(3, 1, 1); stem(random_sequence, 'filled'); title('Входная последовательность'); xlabel('Номер'); ylabel('Значение'); grid on;
    subplot(3, 1, 2); stem(encoded_sequence, 'filled'); title('Кодовая последовательность'); xlabel('Номер'); ylabel('Значение'); grid on;
    subplot(3, 1, 3); stem(decoded_sequence, 'filled'); title('Декодированная последовательность'); xlabel('Номер'); ylabel('Значение'); grid on;

    % --- Одно слово ---
    single_word = random_sequence(1:k);
    encoded_word = bchenco(single_word, n, k);
    received_word = encoded_word;
    decoded_word = bchdeco(received_word, n, t);
    decoded_word = decoded_word(end-k+1:end);

    figure; stem(single_word, 'filled'); title('Входное слово'); xlabel('Номер'); ylabel('Значение'); grid on;
    figure; stem(encoded_word, 'filled'); title('Кодовая последовательность (реализация)'); xlabel('Номер'); ylabel('Значение'); grid on;
    figure; stem(decoded_word, 'filled'); title('Декодированное слово'); xlabel('Номер'); ylabel('Значение'); grid on;

    % --- Разница входа и выхода ---
    difference_sequence = xor(random_sequence, decoded_sequence);
    figure; stem(difference_sequence, 'filled'); title('Сравнение входной и декодированной'); xlabel('Номер'); ylabel('Разница'); grid on;

    % --- Сохраняем CSV для Qt ---
    csvwrite(fullfile(out_dir, 'p2_random.csv'), random_sequence);
    csvwrite(fullfile(out_dir, 'p2_encoded.csv'), encoded_sequence);
    csvwrite(fullfile(out_dir, 'p2_decoded.csv'), decoded_sequence);
    csvwrite(fullfile(out_dir, 'p2_compare.csv'), difference_sequence);
endfunction

% --- Генерируем случайную последовательность ---
random_sequence = round(rand(1, k * num_words_default));

% --- Запуск основной функции ---
run_bch_model(random_sequence, n, k, t, out_dir);
)")
                         .arg(p.n)
                         .arg(p.k)
                         .arg(p.t)
                         .arg(p.numWords)
                         .arg(p.channelErrorProbability)
                         .arg(outDirLocal);

    ts << script;
    f.close();
}

