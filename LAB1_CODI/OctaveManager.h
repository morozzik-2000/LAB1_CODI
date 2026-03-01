#ifndef OCTAVEMANAGER_H
#define OCTAVEMANAGER_H

#include <QString>
#include <QSettings>

class OctaveManager
{
public:
    static OctaveManager& instance()
    {
        static OctaveManager inst;
        return inst;
    }

    void setPath(const QString& path)
    {
        octavePath = path;
        QSettings settings;
        settings.setValue("octavePath", path);
    }

    QString getPath() const
    {
        return octavePath;
    }

    void load()
    {
        QSettings settings;
        octavePath = settings.value("octavePath").toString();
    }

private:
    OctaveManager() { load(); }
    QString octavePath;
};

#endif
