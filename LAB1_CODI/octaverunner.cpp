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
    // ========== 1. БЫСТРЫЙ ПОИСК через системные команды ==========

    // Поиск через 'where' в командной строке
    QProcess which;
    which.start("where octave");
    if (which.waitForFinished(3000)) {
        QString output = which.readAllStandardOutput().trimmed();
        if (!output.isEmpty()) {
            QStringList lines = output.split("\n", Qt::SkipEmptyParts);
            for (const QString &line : lines) {
                QString path = line.trimmed();
                if (QFile::exists(path)) {
                    qDebug() << "Found via 'where':" << path;
                    emit logMessage("Found via 'where':" + path);
                    return QDir::toNativeSeparators(path);
                }
            }
        }
    }

    // Поиск в PATH
    QString pathEnv = qgetenv("PATH");
    QStringList paths = pathEnv.split(";", Qt::SkipEmptyParts);
    for (const QString &path : paths) {
        QString exePath = QDir(path).filePath("octave.exe");
        if (QFile::exists(exePath)) {
            qDebug() << "Found in PATH:" << exePath;
            emit logMessage("Found in PATH:" + exePath);
            return QDir::toNativeSeparators(exePath);
        }
    }

    // ========== 2. ПОЛНЫЙ ПОИСК ПО ВСЕМ ДИСКАМ ==========

#ifdef Q_OS_WIN
    QFileInfoList drives = QDir::drives();

    // Возможные имена папок с Octave
    QStringList possibleFolderNames;
    possibleFolderNames << "GNU Octave"
                        << "Octave"
                        << "octave"
                        << "GNU\\ Octave"
                        << "Octave-*";

    // Возможные подпути до octave.exe
    QStringList possibleSubPaths;
    possibleSubPaths << "mingw64/bin/octave.exe"
                     << "mingw32/bin/octave.exe"
                     << "bin/octave.exe"
                     << "usr/bin/octave.exe"
                     << "octave/bin/octave.exe";

    for (const QFileInfo &drive : drives) {
        QString drivePath = drive.absolutePath();
        if (drivePath.endsWith(":")) drivePath += "/";

        qDebug() << "Searching on drive:" << drivePath;
        emit logMessage("Searching on drive:" + drivePath);

        // Поиск в стандартных местах
        for (const QString &folderName : possibleFolderNames) {
            for (const QString &subPath : possibleSubPaths) {
                // Если в имени папки есть * - ищем все варианты
                if (folderName.contains('*')) {
                    QDir searchDir(drivePath);
                    QStringList foundDirs = searchDir.entryList(QStringList() << folderName,
                                                                QDir::Dirs | QDir::NoDotAndDotDot);
                    for (const QString &foundDir : foundDirs) {
                        QString testPath = drivePath + foundDir + "/" + subPath;
                        if (QFile::exists(testPath)) {
                            qDebug() << "Found in:" << testPath;
                            emit logMessage("Found in:" + testPath);
                            return QDir::toNativeSeparators(testPath);
                        }
                    }
                } else {
                    // Обычная папка
                    QString testPath = drivePath + folderName + "/" + subPath;
                    if (QFile::exists(testPath)) {
                        qDebug() << "Found in:" << testPath;
                        emit logMessage("Found in:" + testPath);
                        return QDir::toNativeSeparators(testPath);
                    }

                    // Вариант с Program Files
                    testPath = drivePath + "Program Files/" + folderName + "/" + subPath;
                    if (QFile::exists(testPath)) {
                        qDebug() << "Found in:" << testPath;
                        emit logMessage("Found in:" + testPath);
                        return QDir::toNativeSeparators(testPath);
                    }

                    // Вариант с Program Files (x86)
                    testPath = drivePath + "Program Files (x86)/" + folderName + "/" + subPath;
                    if (QFile::exists(testPath)) {
                        qDebug() << "Found in:" << testPath;
                        emit logMessage("Found in:" + testPath);
                        return QDir::toNativeSeparators(testPath);
                    }
                }
            }
        }

        // ========== 3. ПОЛНОЕ РЕКУРСИВНОЕ СКАНИРОВАНИЕ (если не нашли) ==========
        // Ограничиваем глубину поиска, чтобы не сканировать весь диск вечно

        qDebug() << "Performing deep search on" << drivePath << "(this may take a moment)...";
        emit logMessage("Performing deep search on" + drivePath);

        QString foundPath = deepSearchForOctave(drivePath);
        if (!foundPath.isEmpty()) {
            qDebug() << "Found via deep search:" << foundPath;
            emit logMessage("Found via deep search:" + foundPath);
            return QDir::toNativeSeparators(foundPath);
        }
    }
#endif

    qDebug() << "Octave executable not found anywhere!";
    emit logMessage("Octave executable not found anywhere!");
    return QString();
}

// Глубокий рекурсивный поиск с ограничением глубины
QString OctaveRunner::deepSearchForOctave(const QString &startDir, int depth)
{
    if (depth > 5) return QString(); // Ограничиваем глубину, чтобы не зависнуть

    QDir dir(startDir);
    if (!dir.exists()) return QString();

    // Проверяем наличие octave.exe в текущей папке
    if (QFile::exists(dir.filePath("octave.exe"))) {
        return dir.filePath("octave.exe");
    }

    // Проверяем в подпапках bin, mingw64/bin и т.д.
    QStringList binPaths;
    binPaths << "bin/octave.exe"
             << "mingw64/bin/octave.exe"
             << "mingw32/bin/octave.exe"
             << "usr/bin/octave.exe"
             << "octave/bin/octave.exe";

    for (const QString &binPath : binPaths) {
        QString testPath = dir.filePath(binPath);
        if (QFile::exists(testPath)) {
            return testPath;
        }
    }

    // Рекурсивно обходим подпапки
    QStringList subDirs = dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);

    // Приоритетные папки для поиска
    QStringList priorityDirs;
    for (const QString &subDir : subDirs) {
        QString lowerDir = subDir.toLower();
        if (lowerDir.contains("octave") || lowerDir.contains("gnu")) {
            priorityDirs << subDir;
        }
    }

    // Сначала ищем в приоритетных папках
    for (const QString &subDir : priorityDirs) {
        QString result = deepSearchForOctave(dir.filePath(subDir), depth + 1);
        if (!result.isEmpty()) return result;
    }

    // Потом в остальных (но только на небольшой глубине)
    if (depth < 3) {
        for (const QString &subDir : subDirs) {
            if (!priorityDirs.contains(subDir)) {
                QString result = deepSearchForOctave(dir.filePath(subDir), depth + 1);
                if (!result.isEmpty()) return result;
            }
        }
    }

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

// void OctaveRunner::runOctave(OctaveParams_ &params)
// {
//     qDebug() << "[runOctave] Created in thread:" << QThread::currentThread();
//     outDir = QDir::toNativeSeparators(QDir::currentPath() + "/results/");
//     QDir().mkpath(outDir);
//     qDebug() << "Results folder:" << outDir;

//     QString scriptPath = QDir::toNativeSeparators(QDir::currentPath() + "/bch_lab_auto.m");
//     writeOctaveScript(params, scriptPath, outDir);

//     QString octaveProgram = findOctaveExecutable();

//     if (octaveProgram.isEmpty()) {
//         emit errorOccurred("❌ Octave executable not found. Please install GNU Octave or specify path manually.");
//         return;
//     }

//     qDebug() << "Using Octave from:" << octaveProgram;

//     QStringList args;
//     args << "--no-gui" << "--silent" << scriptPath;

//     proc = new QProcess(this);
//     connect(proc, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
//             this, [this](int exitCode, QProcess::ExitStatus){
//                 if(exitCode == 0) emit finished();
//                 else emit errorOccurred("Octave finished with error");
//                 proc->deleteLater(); proc = nullptr;
//             });
//     connect(proc, &QProcess::errorOccurred, this, [this](QProcess::ProcessError){
//         emit errorOccurred("Octave launch error");
//         if(proc) { proc->deleteLater(); proc=nullptr; }
//     });
//     connect(proc, &QProcess::readyReadStandardOutput, [this](){
//         QByteArray b = proc->readAllStandardOutput();
//         if(!b.isEmpty()) emit logMessage(QString::fromUtf8(b));
//     });
//     connect(proc, &QProcess::readyReadStandardError, [this](){
//         QByteArray b = proc->readAllStandardError();
//         if(!b.isEmpty()) emit logMessage(QString::fromUtf8(b));
//     });

//     proc->start(octaveProgram, args);
//     if(!proc->waitForStarted(3000)) {
//         emit errorOccurred("🆘 Не удалось подключится к Octave");
//     } else {
//         emit logMessage("⚙ Octave в процессе выполнения...");
//     }
// }

void OctaveRunner::runOctave(OctaveParams_ &params)
{
    qDebug() << "[runOctave] Created in thread:" << QThread::currentThread();

// ===== ИСПРАВЛЕНО: Используем временную папку вместо текущей директории =====
#ifdef Q_OS_WIN
    QString tempPath = QDir::tempPath();  // C:/Users/Имя/AppData/Local/Temp
    outDir = tempPath + "/bch_lab_results/";
#else
    outDir = QDir::home().filePath("bch_lab_results/");
#endif

    // Создаем папку с проверкой
    QDir dir;
    if (!dir.mkpath(outDir)) {
        emit errorOccurred("❌ Не удалось создать папку: " + outDir);
        return;
    }

    // Проверяем, что папка действительно создалась и в нее можно писать
    if (!QFileInfo(outDir).isWritable()) {
        emit errorOccurred("❌ Нет прав на запись в папку: " + outDir);
        return;
    }

    emit logMessage("📁 Папка для результатов: " + outDir);
    qDebug() << "Results folder:" << outDir;

    // ===== ИСПРАВЛЕНО: Создаем скрипт тоже во временной папке =====
    QString scriptPath = QDir::toNativeSeparators(QDir::tempPath() + "/bch_lab_auto.m");

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
                if(exitCode == 0) {
                    emit finished();
                    emit logMessage("✅ Octave выполнен успешно!");
                } else {
                    emit errorOccurred("❌ Octave завершился с ошибкой");
                }
                proc->deleteLater();
                proc = nullptr;
            });
    connect(proc, &QProcess::errorOccurred, this, [this](QProcess::ProcessError error){
        QString errorMsg;
        switch(error) {
        case QProcess::FailedToStart:
            errorMsg = "Не удалось запустить Octave. Проверьте путь.";
            break;
        case QProcess::Crashed:
            errorMsg = "Octave аварийно завершился.";
            break;
        default:
            errorMsg = "Ошибка при запуске Octave.";
        }
        emit errorOccurred("❌ " + errorMsg);
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
        emit errorOccurred("🆘 Не удалось подключиться к Octave");
    } else {
        emit logMessage("⚙️ Octave в процессе выполнения...");
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
