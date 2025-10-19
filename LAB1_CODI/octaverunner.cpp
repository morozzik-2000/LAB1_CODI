#include "OctaveRunner.h"
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QDebug>

OctaveRunner::OctaveRunner(QObject *parent)
    : QObject(parent), proc(nullptr)
{}

void OctaveRunner::runOctave(const OctaveParams &params)
{
    outDir = QDir::toNativeSeparators(QDir::currentPath() + "/results/");
    QDir().mkpath(outDir);
    qDebug() << "Results folder:" << outDir;

    QString scriptPath = QDir::toNativeSeparators(QDir::currentPath() + "/bch_lab_auto.m");
    writeOctaveScript(params, scriptPath, outDir);

    QString octaveProgram = QDir::toNativeSeparators("E:/Octave/Octave-10.2.0/mingw64/bin/octave.exe");

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
        emit errorOccurred("Failed to start Octave");
    } else {
        emit logMessage("Octave started...");
    }
}

void OctaveRunner::writeOctaveScript(const OctaveParams &p, const QString &path, const QString &outDirLocal)
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
part = %6;

out_dir = '%7';
if ~exist(out_dir,'dir'), mkdir(out_dir); end

function save_chart_csv_png(filename, data, out_dir)
    csvwrite(fullfile(out_dir, [filename '.csv']), data(:)');
    figure; stem(data, 'filled');
    saveas(gcf, fullfile(out_dir, [filename '.png']));
    close(gcf);
endfunction

function part1(random_sequence, n, k, t, out_dir)
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

    % Построение трех графиков
    figure;
    subplot(3, 1, 1); stem(random_sequence, 'filled'); title('Входная последовательность'); xlabel('Номер'); ylabel('Значение'); grid on;
    subplot(3, 1, 2); stem(encoded_sequence, 'filled'); title('Кодовая последовательность'); xlabel('Номер'); ylabel('Значение'); grid on;
    subplot(3, 1, 3); stem(decoded_sequence, 'filled'); title('Декодированная последовательность'); xlabel('Номер'); ylabel('Значение'); grid on;

    % Одно слово
    single_word = random_sequence(1:k);
    encoded_word = bchenco(single_word, n, k);
    received_word = encoded_word;
    decoded_word = bchdeco(received_word, n, t);
    decoded_word = decoded_word(end-k+1:end);

    figure; stem(single_word, 'filled'); title('Входное слово'); xlabel('Номер'); ylabel('Значение'); grid on;
    figure; stem(encoded_word, 'filled'); title('Кодовая последовательность (реализация)'); xlabel('Номер'); ylabel('Значение'); grid on;
    figure; stem(decoded_word, 'filled'); title('Декодированное слово'); xlabel('Номер'); ylabel('Значение'); grid on;

    % Разница входа и выхода
    difference_sequence = xor(random_sequence, decoded_sequence);
    figure; stem(difference_sequence, 'filled'); title('Сравнение входной и декодированной'); xlabel('Номер'); ylabel('Разница'); grid on;

    % Сохраняем CSV для Qt
    csvwrite(fullfile(out_dir, 'p2_random.csv'), random_sequence);
    csvwrite(fullfile(out_dir, 'p2_encoded.csv'), encoded_sequence);
    csvwrite(fullfile(out_dir, 'p2_decoded.csv'), decoded_sequence);
    csvwrite(fullfile(out_dir, 'p2_compare.csv'), difference_sequence);
endfunction

% Генерируем случайную последовательность
random_sequence = round(rand(1, k * num_words_default));

switch part
    case 1
        part1(random_sequence, n, k, t, out_dir);
    otherwise
        disp('Выбранная часть не реализована');
endswitch
)").arg(p.n).arg(p.k).arg(p.t).arg(p.numWords).arg(p.errorProb).arg(p.part).arg(outDirLocal);

    ts << script;
    f.close();
}
