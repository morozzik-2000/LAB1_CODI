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
