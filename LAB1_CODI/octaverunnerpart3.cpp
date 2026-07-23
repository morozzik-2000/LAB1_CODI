// // #include "octaverunnerpart3.h"
// // #include <QDir>
// // #include <QFile>
// // #include <QTextStream>
// // #include <QDebug>
// // #include <QThread>

// // OctaveRunnerPart3::OctaveRunnerPart3(QObject *parent)
// //     : QObject(parent), proc(nullptr) {}


// // void OctaveRunnerPart3::run()
// // {
// //     runOctave(params);   // выполняем исходный код
// // }

// // QString OctaveRunnerPart3::findOctaveExecutable()
// // {
// //     QStringList possiblePaths;

// //     // Стандартные пути установки на Windows
// //     possiblePaths << "C:/Program Files/GNU Octave/Octave-*/mingw64/bin/octave.exe";
// //     possiblePaths << "C:/Program Files (x86)/GNU Octave/Octave-*/mingw64/bin/octave.exe";
// //     possiblePaths << "C:/Users/*/AppData/Local/Programs/GNU Octave/Octave-*/mingw64/bin/octave.exe";
// //     possiblePaths << "E:/Octave/Octave-*/mingw64/bin/octave.exe";

// //     // Поиск через where в командной строке
// //     QProcess which;
// //     which.start("where octave");
// //     if (which.waitForFinished(3000)) {
// //         QString output = which.readAllStandardOutput().trimmed();
// //         if (!output.isEmpty()) {
// //             QStringList lines = output.split("\n", Qt::SkipEmptyParts);
// //             for (const QString &line : lines) {
// //                 QString path = line.trimmed();
// //                 if (QFile::exists(path)) {
// //                     qDebug() << "Found Octave via 'where':" << path;
// //                     return QDir::toNativeSeparators(path);
// //                 }
// //             }
// //         }
// //     }

// //     // Поиск в PATH
// //     QString pathEnv = qgetenv("PATH");
// //     QStringList paths = pathEnv.split(";", Qt::SkipEmptyParts);
// //     for (const QString &path : paths) {
// //         QDir dir(path);
// //         QString exePath = dir.filePath("octave.exe");
// //         if (QFile::exists(exePath)) {
// //             qDebug() << "Found Octave in PATH:" << exePath;
// //             return QDir::toNativeSeparators(exePath);
// //         }
// //     }

// //     // Рекурсивный поиск по маске
// //     for (const QString &pattern : possiblePaths) {
// //         // Разбираем путь на части
// //         QString pathPattern = QDir::fromNativeSeparators(pattern);
// //         QStringList parts = pathPattern.split('/');

// //         // Начинаем поиск с корневого каталога
// //         QString currentPath = parts.first();
// //         if (currentPath.isEmpty()) currentPath = "/";

// //         // Рекурсивно ищем файл
// //         QString foundPath = findFileRecursive(currentPath, parts.mid(1));
// //         if (!foundPath.isEmpty()) {
// //             qDebug() << "Found Octave via pattern search:" << foundPath;
// //             return QDir::toNativeSeparators(foundPath);
// //         }
// //     }

// // // Поиск во всех дисках Windows
// // #ifdef Q_OS_WIN
// //     QFileInfoList drives = QDir::drives();
// //     for (const QFileInfo &drive : drives) {
// //         QString searchPath = drive.absolutePath() + "GNU Octave";
// //         QString foundPath = findFileRecursive(searchPath,
// //                                               {"Octave-*", "mingw64", "bin", "octave.exe"});
// //         if (!foundPath.isEmpty()) {
// //             qDebug() << "Found Octave on drive:" << foundPath;
// //             return QDir::toNativeSeparators(foundPath);
// //         }
// //     }
// // #endif

// //     qDebug() << "Octave executable not found";
// //     return QString();
// // }

// // // Вспомогательная рекурсивная функция для поиска по маске
// // QString OctaveRunnerPart3::findFileRecursive(const QString &startPath, const QStringList &patternParts, int depth)
// // {
// //     if (depth >= patternParts.size()) return QString();

// //     QDir dir(startPath);
// //     QString currentPattern = patternParts[depth];

// //     if (depth == patternParts.size() - 1) {
// //         // Последний элемент - ищем файл
// //         if (dir.exists(currentPattern)) {
// //             return dir.filePath(currentPattern);
// //         }
// //         return QString();
// //     }

// //     // Ищем подкаталоги по маске
// //     QStringList filters;
// //     filters << currentPattern;
// //     dir.setFilter(QDir::Dirs | QDir::NoDotAndDotDot);
// //     dir.setNameFilters(filters);

// //     QFileInfoList subDirs = dir.entryInfoList();
// //     for (const QFileInfo &subDir : subDirs) {
// //         QString result = findFileRecursive(
// //             subDir.absoluteFilePath(),
// //             patternParts,
// //             depth + 1
// //             );
// //         if (!result.isEmpty()) {
// //             return result;
// //         }
// //     }

// //     return QString();
// // }

// // void OctaveRunnerPart3::runOctave(OctaveParams_ &params)
// // {
// //     qDebug() << "[runOctave] Created in thread:" << QThread::currentThread();
// //     outDir = QDir::toNativeSeparators(QDir::currentPath() + "/results/");
// //     QDir().mkpath(outDir);
// //     qDebug() << "Results folder:" << outDir;

// //     QString scriptPath = QDir::toNativeSeparators(QDir::currentPath() + "/bch_lab_auto.m");
// //     writeOctaveScript(params, scriptPath, outDir);

// //     QString octaveProgram = findOctaveExecutable();

// //     if (octaveProgram.isEmpty()) {
// //         emit errorOccurred("❌ Octave executable not found. Please install GNU Octave or specify path manually.");
// //         return;
// //     }

// //     qDebug() << "Using Octave from:" << octaveProgram;

// //     QStringList args;
// //     args << "--no-gui" << "--silent" << scriptPath;

// //     proc = new QProcess(this);
// //     connect(proc, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
// //             this, [this](int exitCode, QProcess::ExitStatus){
// //                 if(exitCode == 0) emit finished();
// //                 else emit errorOccurred("Octave finished with error");
// //                 proc->deleteLater(); proc = nullptr;
// //             });
// //     connect(proc, &QProcess::errorOccurred, this, [this](QProcess::ProcessError){
// //         emit errorOccurred("Octave launch error");
// //         if(proc) { proc->deleteLater(); proc=nullptr; }
// //     });
// //     connect(proc, &QProcess::readyReadStandardOutput, [this](){
// //         QByteArray b = proc->readAllStandardOutput();
// //         if(!b.isEmpty()) emit logMessage(QString::fromUtf8(b));
// //     });
// //     connect(proc, &QProcess::readyReadStandardError, [this](){
// //         QByteArray b = proc->readAllStandardError();
// //         if(!b.isEmpty()) emit logMessage(QString::fromUtf8(b));
// //     });

// //     proc->start(octaveProgram, args);
// //     if(!proc->waitForStarted(3000)) {
// //         emit errorOccurred("🆘 Не удалось подключится к Octave");
// //     } else {
// //         emit logMessage("⚙ Octave в процессе выполнения...");
// //     }
// // }

// // void OctaveRunnerPart3::writeOctaveScript(OctaveParams_ &p, const QString &path, const QString &outDirLocal)
// // {

// //     QFile f(path);
// //     if(!f.open(QIODevice::WriteOnly | QIODevice::Text)) return;
// //     QTextStream ts(&f);

// //     QString script = QString(R"(
// // close all; clear;
// // pkg load communications;

// // n = %1; k = %2; t = %3; num_words = %4;
// // channel_error_probability = %5;
// // out_dir = '%6';
// // if ~exist(out_dir,'dir'), mkdir(out_dir); end

// // function part3(n, k, t, channel_error_probability, num_words, out_dir)
// //     random_sequence = round(rand(1, k*num_words));
// //     encoded_sequence = [];
// //     for i = 1:k:length(random_sequence)
// //         current_chunk = random_sequence(i:i+k-1);
// //         encoded_msg = bchenco(current_chunk, n, k);
// //         encoded_sequence = [encoded_sequence encoded_msg];
// //     end

// //     received_sequence = encoded_sequence;
// //     error_vector = rand(1,length(received_sequence)) < channel_error_probability;
// //     received_sequence(error_vector) = ~received_sequence(error_vector);

// //     decoded_sequence_3 = [];
// //     for i = 1:n:length(received_sequence)
// //         current_block = received_sequence(i:i+n-1);
// //         decoded_msg = bchdeco(current_block, n, t);
// //         decoded_sequence_3 = [decoded_sequence_3 decoded_msg(end-k+1:end)];
// //     end

// //     difference_sequence_3 = random_sequence ~= decoded_sequence_3;
// //     num_errors_3 = sum(difference_sequence_3);
// //     sequence_length = length(decoded_sequence_3);

// //     figure; stem(random_sequence,'filled'); title('Информационная последовательность'); xlabel('Номер'); ylabel('Значение'); grid on;
// //     figure; stem(decoded_sequence_3,'filled'); title('Выход декодера'); xlabel('Номер'); ylabel('Значение'); grid on;
// //     figure; stem(difference_sequence_3,'filled'); title('Сравнение информационной и декодированной'); xlabel('Номер'); ylabel('Результат'); grid on;

// //     fprintf('QQQ: %d\n', num_errors_3);

// //     csvwrite(fullfile(out_dir,'p4_encoded.csv'), encoded_sequence);
// //     csvwrite(fullfile(out_dir,'p4_received.csv'), received_sequence);
// //     csvwrite(fullfile(out_dir,'p4_decoded.csv'), decoded_sequence_3);
// //     csvwrite(fullfile(out_dir,'p4_difference.csv'), difference_sequence_3);
// // endfunction

// // part3(n, k, t, channel_error_probability, num_words, out_dir);

// // )").arg(p.n).arg(p.k).arg(p.t).arg(p.numWords)
// //                          .arg(QString::number(p.channelErrorProbability, 'f', 6))
// //                          .arg(outDirLocal);


// //     ts << script;
// //     f.close();
// // }
#include "octaverunnerpart3.h"
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QThread>

OctaveRunnerPart3::OctaveRunnerPart3(QObject *parent)
    : QObject(parent), proc(nullptr) {}


void OctaveRunnerPart3::run()
{
    runOctave(params);   // выполняем исходный код
}

void OctaveRunnerPart3::runOctave(OctaveParams_ &params)
{
    qDebug() << "[runOctave] Created in thread:" << QThread::currentThread();
    outDir = QDir::toNativeSeparators(QDir::currentPath() + "/results/");
    QDir().mkpath(outDir);
    qDebug() << "Results folder:" << outDir;

    QString scriptPath = QDir::toNativeSeparators(QDir::currentPath() + "/bch_lab_part3.m");
    writeOctaveScript(params, scriptPath, outDir);
    // QString octaveProgram = QDir::toNativeSeparators("C:/Program Files/GNU Octave/Octave-8.3.0/mingw64/bin/octave.exe");
    QString octaveProgram = QDir::toNativeSeparators("E:/Octave/Octave-10.2.0/mingw64/bin/octave.exe");
    // QString octaveProgram = QDir::toNativeSeparators("C:/Users/Student/AppData/Local/Programs/GNU Octave/Octave-10.2.0/mingw64/bin/octave.exe");

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

void OctaveRunnerPart3::writeOctaveScript(OctaveParams_ &p, const QString &path, const QString &outDirLocal)
{

    QFile f(path);
    if(!f.open(QIODevice::WriteOnly | QIODevice::Text)) return;
    QTextStream ts(&f);

    QString script = QString(R"(
close all; clear;
pkg load communications;

n = %1; k = %2; t = %3; num_words = %4;
channel_error_probability = %5;
out_dir = '%6';
if ~exist(out_dir,'dir'), mkdir(out_dir); end

function part3(n, k, t, channel_error_probability, num_words, out_dir)
    random_sequence = round(rand(1, k*num_words));
    encoded_sequence = [];
    for i = 1:k:length(random_sequence)
        current_chunk = random_sequence(i:i+k-1);
        encoded_msg = bchenco(current_chunk, n, k);
        encoded_sequence = [encoded_sequence encoded_msg];
    end

    received_sequence = encoded_sequence;
    error_vector = rand(1,length(received_sequence)) < channel_error_probability;
    received_sequence(error_vector) = ~received_sequence(error_vector);

    decoded_sequence_3 = [];
    for i = 1:n:length(received_sequence)
        current_block = received_sequence(i:i+n-1);
        decoded_msg = bchdeco(current_block, n, t);
        decoded_sequence_3 = [decoded_sequence_3 decoded_msg(end-k+1:end)];
    end

    difference_sequence_3 = random_sequence ~= decoded_sequence_3;
    num_errors_3 = sum(difference_sequence_3);
    sequence_length = length(decoded_sequence_3);

    figure; stem(random_sequence,'filled'); title('Информационная последовательность'); xlabel('Номер'); ylabel('Значение'); grid on;
    figure; stem(decoded_sequence_3,'filled'); title('Выход декодера'); xlabel('Номер'); ylabel('Значение'); grid on;
    figure; stem(difference_sequence_3,'filled'); title('Сравнение информационной и декодированной'); xlabel('Номер'); ylabel('Результат'); grid on;

    fprintf('QQQ: %d\n', num_errors_3);

    csvwrite(fullfile(out_dir,'p4_encoded.csv'), encoded_sequence);
    csvwrite(fullfile(out_dir,'p4_received.csv'), received_sequence);
    csvwrite(fullfile(out_dir,'p4_decoded.csv'), decoded_sequence_3);
    csvwrite(fullfile(out_dir,'p4_difference.csv'), difference_sequence_3);
endfunction

part3(n, k, t, channel_error_probability, num_words, out_dir);

)").arg(p.n).arg(p.k).arg(p.t).arg(p.numWords)
                         .arg(QString::number(p.channelErrorProbability, 'f', 6))
                         .arg(outDirLocal);


    ts << script;
    f.close();
}
// #include "octaverunnerpart3.h"
// #include <QDir>
// #include <QFile>
// #include <QTextStream>
// #include <QDebug>
// #include <QThread>

// OctaveRunnerPart3::OctaveRunnerPart3(QObject *parent)
//     : QObject(parent), proc(nullptr) {}


// void OctaveRunnerPart3::run()
// {
//     runOctave(params);   // выполняем исходный код
// }

// QString OctaveRunnerPart3::findOctaveExecutable()
// {
//     QStringList possiblePaths;

//     // Стандартные пути установки на Windows
//     possiblePaths << "C:/Program Files/GNU Octave/Octave-*/mingw64/bin/octave.exe";
//     possiblePaths << "C:/Program Files (x86)/GNU Octave/Octave-*/mingw64/bin/octave.exe";
//     possiblePaths << "C:/Users/*/AppData/Local/Programs/GNU Octave/Octave-*/mingw64/bin/octave.exe";
//     possiblePaths << "D:/Program Files/GNU Octave/Octave-*/mingw64/bin/octave.exe";
//     possiblePaths << "E:/Program Files/GNU Octave/Octave-*/mingw64/bin/octave.exe";

//     // Поиск через where в командной строке
//     QProcess which;
//     which.start("where octave");
//     if (which.waitForFinished(3000)) {
//         QString output = which.readAllStandardOutput().trimmed();
//         if (!output.isEmpty()) {
//             QStringList lines = output.split("\n", Qt::SkipEmptyParts);
//             for (const QString &line : lines) {
//                 QString path = line.trimmed();
//                 if (QFile::exists(path)) {
//                     qDebug() << "Found Octave via 'where':" << path;
//                     return QDir::toNativeSeparators(path);
//                 }
//             }
//         }
//     }

//     // Поиск в PATH
//     QString pathEnv = qgetenv("PATH");
//     QStringList paths = pathEnv.split(";", Qt::SkipEmptyParts);
//     for (const QString &path : paths) {
//         QDir dir(path);
//         QString exePath = dir.filePath("octave.exe");
//         if (QFile::exists(exePath)) {
//             qDebug() << "Found Octave in PATH:" << exePath;
//             return QDir::toNativeSeparators(exePath);
//         }
//     }

//     // Рекурсивный поиск по маске
//     for (const QString &pattern : possiblePaths) {
//         QString foundPath = findPathWithWildcards(pattern);
//         if (!foundPath.isEmpty()) {
//             qDebug() << "Found Octave via pattern search:" << foundPath;
//             return QDir::toNativeSeparators(foundPath);
//         }
//     }

//     // Поиск во всех дисках Windows
// #ifdef Q_OS_WIN
//     QFileInfoList drives = QDir::drives();
//     for (const QFileInfo &drive : drives) {
//         QString drivePath = drive.absolutePath();
//         QString searchPattern = drivePath + "/Program Files/GNU Octave/Octave-*/mingw64/bin/octave.exe";
//         QString foundPath = findPathWithWildcards(searchPattern);
//         if (!foundPath.isEmpty()) {
//             qDebug() << "Found Octave on drive:" << foundPath;
//             return QDir::toNativeSeparators(foundPath);
//         }

//         // Проверяем и Program Files (x86)
//         searchPattern = drivePath + "/Program Files (x86)/GNU Octave/Octave-*/mingw64/bin/octave.exe";
//         foundPath = findPathWithWildcards(searchPattern);
//         if (!foundPath.isEmpty()) {
//             qDebug() << "Found Octave on drive:" << foundPath;
//             return QDir::toNativeSeparators(foundPath);
//         }
//     }
// #endif

//     qDebug() << "Octave executable not found";
//     return QString();
// }

// // Новая функция для поиска с поддержкой wildcards
// QString OctaveRunnerPart3::findPathWithWildcards(const QString &pattern)
// {
//     QString normalizedPattern = QDir::fromNativeSeparators(pattern);

//     // Если нет wildcards, просто проверяем существование
//     if (!normalizedPattern.contains('*') && !normalizedPattern.contains('?')) {
//         return QFile::exists(normalizedPattern) ? normalizedPattern : QString();
//     }

//     // Разделяем путь на части
//     QStringList parts = normalizedPattern.split('/', Qt::SkipEmptyParts);
//     if (parts.isEmpty()) return QString();

//     // Обрабатываем корневой каталог (C:, D: и т.д.)
//     QString rootPath = parts.first();
//     if (rootPath.endsWith(':')) {
//         rootPath += "/";  // Превращаем "C:" в "C:/"
//     }

//     // Начинаем рекурсивный поиск
//     return findFileRecursive(rootPath, parts.mid(1));
// }

// // Исправленная рекурсивная функция
// QString OctaveRunnerPart3::findFileRecursive(const QString &currentPath, const QStringList &remainingParts, int depth)
// {
//     // Если все части обработаны, возвращаем текущий путь
//     if (depth >= remainingParts.size()) {
//         return QFile::exists(currentPath) ? currentPath : QString();
//     }

//     QDir dir(currentPath);
//     if (!dir.exists()) return QString();

//     QString currentPattern = remainingParts[depth];
//     bool hasWildcards = currentPattern.contains('*') || currentPattern.contains('?');

//     // Если нет wildcards, просто идём дальше
//     if (!hasWildcards) {
//         QString nextPath = dir.filePath(currentPattern);
//         return findFileRecursive(nextPath, remainingParts, depth + 1);
//     }

//     // С wildcards - ищем все подходящие файлы/папки
//     QStringList filters;
//     filters << currentPattern;

//     // Определяем, что ищем
//     if (depth == remainingParts.size() - 1) {
//         // Последний элемент - ищем файлы
//         dir.setFilter(QDir::Files | QDir::NoDotAndDotDot);
//     } else {
//         // Ищем папки
//         dir.setFilter(QDir::Dirs | QDir::NoDotAndDotDot);
//     }

//     dir.setNameFilters(filters);
//     QFileInfoList entries = dir.entryInfoList();

//     for (const QFileInfo &entry : entries) {
//         QString result = findFileRecursive(entry.absoluteFilePath(), remainingParts, depth + 1);
//         if (!result.isEmpty()) {
//             return result;
//         }
//     }

//     return QString();
// }
// void OctaveRunnerPart3::runOctave(OctaveParams_ &params)
// {
//     qDebug() << "[runOctave] Created in thread:" << QThread::currentThread();
// // ===== ИСПРАВЛЕНО: Используем временную папку вместо текущей директории =====
// #ifdef Q_OS_WIN
//     QString tempPath = QDir::tempPath();  // C:/Users/Имя/AppData/Local/Temp
//     outDir = tempPath + "/bch_lab_results/";
// #else
//     outDir = QDir::home().filePath("bch_lab_results/");
// #endif

//     // Создаем папку с проверкой
//     QDir dir;
//     if (!dir.mkpath(outDir)) {
//         emit errorOccurred("❌ Не удалось создать папку: " + outDir);
//         return;
//     }

//     // Проверяем, что папка действительно создалась и в нее можно писать
//     if (!QFileInfo(outDir).isWritable()) {
//         emit errorOccurred("❌ Нет прав на запись в папку: " + outDir);
//         return;
//     }

//     emit logMessage("📁 Папка для результатов: " + outDir);
//     qDebug() << "Results folder:" << outDir;

//     // ===== ИСПРАВЛЕНО: Создаем скрипт тоже во временной папке =====
//     QString scriptPath = QDir::toNativeSeparators(QDir::tempPath() + "/bch_lab_auto.m");

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

// void OctaveRunnerPart3::writeOctaveScript(OctaveParams_ &p, const QString &path, const QString &outDirLocal)
// {

//     QFile f(path);
//     if(!f.open(QIODevice::WriteOnly | QIODevice::Text)) return;
//     QTextStream ts(&f);

//     QString script = QString(R"(
//  close all; clear;
//  pkg load communications;

//  n = %1; k = %2; t = %3; num_words = %4;
//  channel_error_probability = %5;
//  out_dir = '%6';
//  if ~exist(out_dir,'dir'), mkdir(out_dir); end

//  function part3(n, k, t, channel_error_probability, num_words, out_dir)
//      random_sequence = round(rand(1, k*num_words));
//      encoded_sequence = [];
//      for i = 1:k:length(random_sequence)
//          current_chunk = random_sequence(i:i+k-1);
//          encoded_msg = bchenco(current_chunk, n, k);
//          encoded_sequence = [encoded_sequence encoded_msg];
//      end

//      received_sequence = encoded_sequence;
//      error_vector = rand(1,length(received_sequence)) < channel_error_probability;
//      received_sequence(error_vector) = ~received_sequence(error_vector);

//      decoded_sequence_3 = [];
//      for i = 1:n:length(received_sequence)
//          current_block = received_sequence(i:i+n-1);
//          decoded_msg = bchdeco(current_block, n, t);
//          decoded_sequence_3 = [decoded_sequence_3 decoded_msg(end-k+1:end)];
//      end

//      difference_sequence_3 = random_sequence ~= decoded_sequence_3;
//      num_errors_3 = sum(difference_sequence_3);
//      sequence_length = length(decoded_sequence_3);

//      figure; stem(random_sequence,'filled'); title('Информационная последовательность'); xlabel('Номер'); ylabel('Значение'); grid on;
//      figure; stem(decoded_sequence_3,'filled'); title('Выход декодера'); xlabel('Номер'); ylabel('Значение'); grid on;
//      figure; stem(difference_sequence_3,'filled'); title('Сравнение информационной и декодированной'); xlabel('Номер'); ylabel('Результат'); grid on;

//      fprintf('QQQ: %d\n', num_errors_3);

//      csvwrite(fullfile(out_dir,'p4_encoded.csv'), encoded_sequence);
//      csvwrite(fullfile(out_dir,'p4_received.csv'), received_sequence);
//      csvwrite(fullfile(out_dir,'p4_decoded.csv'), decoded_sequence_3);
//      csvwrite(fullfile(out_dir,'p4_difference.csv'), difference_sequence_3);
//  endfunction

//  part3(n, k, t, channel_error_probability, num_words, out_dir);

//  )").arg(p.n).arg(p.k).arg(p.t).arg(p.numWords)
//                          .arg(QString::number(p.channelErrorProbability, 'f', 6))
//                          .arg(outDirLocal);


//     ts << script;
//     f.close();
// }








