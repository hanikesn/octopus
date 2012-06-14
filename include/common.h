#ifndef COMMON_H
#define COMMON_H

#include <QFlags>
#include <QString>
#include <boost/chrono.hpp>
#include <QDebug>

namespace Data
{
    enum Property {
        INTERPOLATABLE  =1,
        STATEFUL        =2
    };
    Q_DECLARE_FLAGS(Properties, Property)
}

Q_DECLARE_OPERATORS_FOR_FLAGS(Data::Properties)

inline QString fromStdString(std::string const& str)
{
    return QString::fromUtf8(str.c_str(), str.length());
}

inline std::string toStdString(QString const& str)
{
    auto const& data = str.toUtf8();
    return std::string(data.constData(), data.length());
}

class Measurement
{
	typedef boost::chrono::high_resolution_clock Clock;
	Q_DISABLE_COPY(Measurement)
public:
	Measurement(QString const& name) : name(name), start(Clock::now()) {}
	~Measurement()
	{
		qDebug() << name << ": " << boost::chrono::duration_cast<boost::chrono::nanoseconds>(Clock::now() - start).count() << "ns";
	}

private:
	const QString name;
	Clock::time_point start;
};

#endif // COMMON_H
