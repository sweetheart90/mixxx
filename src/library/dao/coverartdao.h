#ifndef COVERARTDAO_H
#define COVERARTDAO_H

#include <QObject>
#include <QSqlDatabase>

#include "library/dao/dao.h"
#include "trackinfoobject.h"

class CoverArtDAO : public QObject, public virtual DAO {
    Q_OBJECT
  public:
    CoverArtDAO(QSqlDatabase& database, ConfigObject<ConfigValue> *pConfig);
    virtual ~CoverArtDAO();
    void setDatabase(QSqlDatabase& database) { m_database = database; }

    void initialize();

    void saveCoverArt(TrackInfoObject*);

   private:
    ConfigObject<ConfigValue>* m_pConfig;
    QSqlDatabase& m_database;

    QString getStoragePath() const;
};

#endif // COVERARTDAO_H
