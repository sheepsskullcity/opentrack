#pragma once

#include "connector.hpp"

#include "bundle.hpp"
#include "slider.hpp"
#include <type_traits>
#include <QString>
#include <QPointF>
#include <QList>

#define OPENTRACK_DEFINE_SLOT(t) void setValue(t datum) { store(datum); }
#define OPENTRACK_DEFINE_SIGNAL(t) void valueChanged(t) const
namespace options {

namespace detail {
template<typename t> struct value_type_traits { using type = t;};
template<> struct value_type_traits<QString> { using type = const QString&; };
template<> struct value_type_traits<slider_value> { using type = const slider_value&; };
template<typename u> struct value_type_traits<QList<u>>
{
    using type = const QList<u>&;
};
template<typename t> using value_type_t = typename value_type_traits<t>::type;
}

class OPENTRACK_OPTIONS_EXPORT base_value : public QObject
{
    Q_OBJECT
public:
    QString name() const { return self_name; }
    base_value(bundle b, const QString& name) : b(b), self_name(name)
    {
        b->on_bundle_created(name, this);
    }
signals:
    OPENTRACK_DEFINE_SIGNAL(double);
    OPENTRACK_DEFINE_SIGNAL(float);
    OPENTRACK_DEFINE_SIGNAL(int);
    OPENTRACK_DEFINE_SIGNAL(bool);
    OPENTRACK_DEFINE_SIGNAL(const QString&);
    OPENTRACK_DEFINE_SIGNAL(const slider_value&);
    OPENTRACK_DEFINE_SIGNAL(const QPointF&);

    OPENTRACK_DEFINE_SIGNAL(const QList<double>&);
    OPENTRACK_DEFINE_SIGNAL(const QList<float>&);
    OPENTRACK_DEFINE_SIGNAL(const QList<int>&);
    OPENTRACK_DEFINE_SIGNAL(const QList<bool>&);
    OPENTRACK_DEFINE_SIGNAL(const QList<QString>&);
    OPENTRACK_DEFINE_SIGNAL(const QList<slider_value>&);
    OPENTRACK_DEFINE_SIGNAL(const QList<QPointF>&);
protected:
    bundle b;
    QString self_name;

    template<typename t>
    void store(const t& datum)
    {
        b->store_kv(self_name, QVariant::fromValue(datum));
    }

    void store(float datum)
    {
        store(double(datum));
    }

public slots:
    OPENTRACK_DEFINE_SLOT(double)
    OPENTRACK_DEFINE_SLOT(int)
    OPENTRACK_DEFINE_SLOT(bool)
    OPENTRACK_DEFINE_SLOT(const QString&)
    OPENTRACK_DEFINE_SLOT(const slider_value&)
    OPENTRACK_DEFINE_SLOT(const QPointF&)

    OPENTRACK_DEFINE_SLOT(const QList<double>&)
    OPENTRACK_DEFINE_SLOT(const QList<float>&)
    OPENTRACK_DEFINE_SLOT(const QList<int>&)
    OPENTRACK_DEFINE_SLOT(const QList<bool>&)
    OPENTRACK_DEFINE_SLOT(const QList<QString>&)
    OPENTRACK_DEFINE_SLOT(const QList<slider_value>&)
    OPENTRACK_DEFINE_SLOT(const QList<QPointF>&)
    public slots:
        virtual void reload() = 0;
        virtual void bundle_value_changed() const = 0;
};

namespace detail {
template<typename t>
struct value_get_traits
{
    static inline t get(const t& val, const t&)
    {
        return val;
    }
};

template<>
struct value_get_traits<slider_value>
{
    using t = slider_value;
    static inline t get(const t& val, const t& def)
    {
        return t(val.cur(), def.min(), def.max());
    }
};

template<typename t, typename Enable = void>
struct value_element_type
{
    using type = typename std::remove_reference<typename std::remove_cv<t>::type>::type;
};

// Qt uses int a lot in slots so use it for all enums
template<typename t>
struct value_element_type<t, typename std::enable_if<std::is_enum<t>::value>::type>
{
    using type = int;
};

template<typename t> using value_element_type_t = typename value_element_type<t>::type;

}

template<typename t>
class value final : public base_value
{
public:
    using element_type = detail::value_element_type_t<t>;

    t operator=(const t& datum)
    {
        store(static_cast<element_type>(datum));
        return datum;
    }

    static constexpr const Qt::ConnectionType DIRECT_CONNTYPE = Qt::AutoConnection;
    static constexpr const Qt::ConnectionType SAFE_CONNTYPE = Qt::QueuedConnection;

    value(bundle b, const QString& name, t def) : base_value(b, name), def(def)
    {
        QObject::connect(b.get(), SIGNAL(reloading()),
                         this, SLOT(reload()),
                         DIRECT_CONNTYPE);
        if (!b->contains(name) || b->get<QVariant>(name).type() == QVariant::Invalid)
            *this = def;
    }

    value(bundle b, const char* name, t def) : value(b, QString(name), def)
    {
    }

    t get() const
    {
        t val = b->contains(self_name)
                ? static_cast<t>(b->get<element_type>(self_name))
                : def;
        return detail::value_get_traits<t>::get(val, def);
    }

    operator t() const { return get(); }

    void reload() override
    {
        *this = static_cast<t>(*this);
    }

    void bundle_value_changed() const override
    {
        emit valueChanged(static_cast<detail::value_type_t<t>>(get()));
    }

private:
    t def;
};


}
