#pragma once

#include "group.hpp"
#include "connector.hpp"

#include <memory>
#include <tuple>
#include <map>
#include <memory>
#include <vector>

#include <QObject>
#include <QString>
#include <QVariant>
#include <QMutex>
#include <QMutexLocker>

#include <QDebug>

#include "compat/util.hpp"
#include "export.hpp"

namespace options {

namespace detail {

class OPENTRACK_OPTIONS_EXPORT bundle final : public QObject, public virtual connector
{
    Q_OBJECT
private:
    QMutex mtx;
    const QString group_name;
    group saved;
    group transient;

    bundle(const bundle&) = delete;
    bundle& operator=(const bundle&) = delete;
    QMutex* get_mtx() override { return &mtx; }

signals:
    void reloading();
    void saving() const;
public:
    bundle(const QString& group_name);
    ~bundle() override {}
    QString name() { return group_name; }
    void reload();
    void store_kv(const QString& name, const QVariant& datum);
    bool contains(const QString& name) const;
    void save();
    void save_deferred(QSettings& s);
    bool modifiedp() const;

    template<typename t>
    t get(const QString& name) const
    {
        QMutexLocker l(const_cast<QMutex*>(&mtx));
        return transient.get<t>(name);
    }
};

struct OPENTRACK_OPTIONS_EXPORT bundler
{
public:
    using k = QString;
    using v = bundle;
    using cnt = int;
    using tt = std::tuple<cnt, std::weak_ptr<v>>;
private:
    QMutex implsgl_mtx;
    std::map<k, tt> implsgl_data;
    void after_profile_changed_();
public:
    bundler();
    ~bundler();
    std::shared_ptr<v> make_bundle(const k& key);
    void bundle_decf(const k& key);
    static void refresh_all_bundles();
};

OPENTRACK_OPTIONS_EXPORT bundler& singleton();
}

using bundle_type = detail::bundle;
using bundle = std::shared_ptr<bundle_type>;

inline bundle make_bundle(const QString& name)
{
    return detail::singleton().make_bundle(name);
}

}
