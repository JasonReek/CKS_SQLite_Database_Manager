#include "dataformat.h"

DataFormat::DataFormat()
{

}

QString DataFormat::toDatetime(QString date_format, QString date)
{
    QStringList date_list;
    QStringList date_format_list;
    // Remove any extra space characters.
    QString f_date = date.replace(" ", "");

    // Check to make sure date value is valid.
    // WARNING: Doesn't catch every case!
    if(!date.contains('/') && !date.contains('-') && !date.contains(','))
        return date;

    // Find out how the date string is put together.
    if(date_format.contains('/'))
    {
        date_list = f_date.split('/');
        date_format_list = date_format.split('/');
    }
    else if(date_format.contains('-'))
    {
        date_list = f_date.split('-');
        date_format_list = date_format.split('-');
    }
    else if(date_format.contains(','))
    {
        date_list = f_date.split(',');
        date_format_list = date_format.split(',');
    }
    //Format to YYYY-MM-DD
    return DataFormat::formatDate(date_format_list, date_list, date);
}

QString DataFormat::formatDate(QStringList date_format_list, QStringList date_list, QString original_value)
{
    QString month_format;
    QString day_format;
    //Format to YYYY-MM-DD
    QString date = "";
    QString dt_f_1 = date_format_list[0];
    QString dt_f_2 = date_format_list[1];
    QString dt_f_3 = date_format_list[2];
    QString dt_1 = date_list[0];
    QString dt_2 = date_list[1];
    QString dt_3 = date_list[2];

    // First get the year
    if(dt_f_1 == DateFormats::YYYY && dt_1.length() == dt_f_1.length())
        date.append(dt_1);
    else if(dt_f_2 == DateFormats::YYYY && dt_2.length() == dt_f_2.length())
        date.append(dt_2);
    else if(dt_f_3 == DateFormats::YYYY && dt_3.length() == dt_f_3.length())
        date.append(dt_3);
    // Not a valid date value
    else
        return original_value;
    date.append('-');

    // Next let's find out which format is being used.
    if(date_format_list.contains(DateFormats::M))
        month_format = DateFormats::M;
    else if(date_format_list.contains(DateFormats::MM))
        month_format = DateFormats::MM;
    else if(date_format_list.contains(DateFormats::MON))
        month_format = DateFormats::MON;
    else
        month_format = DateFormats::MONTH;

    // Now get the month
    if(dt_f_1 == month_format && dt_1.length() == dt_f_1.length())
        date.append(DataFormat::monthFormat(month_format, dt_1));
    else if(dt_f_2 == month_format && dt_2.length() == dt_f_2.length())
        date.append(DataFormat::monthFormat(month_format, dt_2));
    else if(dt_f_3 == month_format && dt_3.length() == dt_f_3.length())
        date.append(DataFormat::monthFormat(month_format, dt_3));
    // Not a valid date value
    else
        return original_value;
    date.append('-');

    // Let's find out which format is being used for the day.
    if(date_format_list.contains(DateFormats::D))
        day_format = DateFormats::D;
    else
        day_format = DateFormats::DD;

    // Finally, we get the day
    if(dt_f_1 == day_format && dt_1.length() == dt_f_1.length())
        date.append(DataFormat::dayFormat(day_format, dt_1));
    else if(dt_f_2 == day_format && dt_2.length() == dt_f_2.length())
        date.append(DataFormat::dayFormat(day_format, dt_2));
    else if(dt_f_3 == day_format && dt_3.length() == dt_f_3.length())
        date.append(DataFormat::dayFormat(day_format, dt_3));
    // Not a valid date value
    else
        return original_value;

    return date;
}

QString DataFormat::monthFormat(QString month_format, QString month)
{
    QString f_month = month;

    if(month_format == DateFormats::M)
    {
        if(month.length() == 1)
            f_month = "0"+month;
    }
    else if(month_format == DateFormats::MON)
    {
        if(DateFormats::MONTH_ABRV_TO_NUMBER.contains(month))
            f_month = DateFormats::MONTH_ABRV_TO_NUMBER[month];
    }
    else if(month_format == DateFormats::MONTH)
    {
        if(DateFormats::MONTH_TO_NUMBER.contains(month))
            f_month = DateFormats::MONTH_TO_NUMBER[month];
    }
    return f_month;
}

QString DataFormat::dayFormat(QString day_format, QString day)
{
    QString f_day = day;

    if(day_format == DateFormats::D)
    {
        if(day.length() == 1)
            f_day = "0"+day;
    }
    return f_day;
}
