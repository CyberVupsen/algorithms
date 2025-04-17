#include <string>
#include <map>
#include <iostream>
#include <vector>
#include <algorithm>

// Month
enum class Month
{
    jan = 1, feb, mar, apr, may, jun,
    jul, aug, sep, oct, nov, dec
};

Month operator ++ (Month& m)
{
    m = (m == Month::dec) ? Month::jan : Month(int(m) + 1);
    return m;
}

// Date
class Date
{
public:
    Date(int d, Month m, int y)
        : d{d}, m{m}, y{y}
        {}
    Date(): d{0}, m{Month::jan}, y{0} {}

    int day() const { return d; }
    Month month() const { return m; }
    int year() const { return y; }

protected:
    int d;
    Month m;
    int y;
};

bool operator < (const Date& d1, const Date& d2)
{
    if (d1.year() > d2.year())
        return false;
    else 
    {
        if(d1.year() == d2.year() && d1.month() > d2.month())
            return false;
        else
        {
            if(d1.year() == d2.year() && d1.month() == d2.month() && d1.day() >= d2.day())
                return false;
        }
    }
    return true;
}

bool leapyear(unsigned y)
{
    if(y % 4 == 0)
    {
        if(y % 100 == 0)
        {
            if(y % 400 != 0)
                return false;
            return true;
        }
        return true;
    }
    return false;
}

int days_in__month(Month month, int year)
{
    int days_in_month = 31;

    switch(month)
    {
    case Month::feb:
        days_in_month = leapyear(year) ? 29 : 28;
        break;
    case Month::apr:
    case Month::jun:
    case Month::sep:
    case Month::nov:
        days_in_month = 30;
        break;
    }
    return days_in_month;
}

Date operator ++ (Date& d)
{
    int day = d.day();
    Month month = d.month();
    int year = d.year();
    int days_in_month = days_in__month(month, year);
    ++day;
    if(day > days_in_month)
    {
        day = 1;
        ++month;
        if(month == Month::jan)
            ++year;
    }
    d = Date(day, month, year);
    return d;
}


std::map<Date, int> make_dates_map(Date start_date, Date end_date)
{
    std::map<Date, int> dates;
    Date current_date = start_date;

    while (current_date < end_date)
    {
        dates[current_date] = 0;
        ++current_date;
    }
    return dates;
}

std::map<Date, int> add_meet_marks(Date start_date, Date end_date, std::map<Date, int>& dates)
{
    Date current_date = start_date;

    while (current_date < end_date)
    {
        dates[current_date] += 1;
        ++current_date;
    }
    return dates;
}

int get_map_max(std::map<Date, int> mapp)
{
    int maxValue = 0;
 
    for (const auto &entry : mapp){
        if (maxValue < entry.second)
            maxValue = entry.second;
    }
    return maxValue;
}


int main()
{
    int n {0};
    std::cin >> n;

    std::vector<Date> dates_born{};
    std::vector<Date> dates_death{};

    Date min_born_date{};
    Date max_death_date{};

    for (int i = 0; i < n; ++i) {
        int day_b {0};
        int month_b {0};
        int year_b {0};
        int day_d {0};
        int month_d {0};
        int year_d {0};

        std::cin >> day_b >> month_b >> year_b >> day_d >> month_d >> year_d;
        Date born_date = Date(day_b, Month(month_b), year_b);
        Date death_date = Date(day_d, Month(month_d), year_d);
        dates_born.push_back(born_date);
        dates_death.push_back(death_date);

        if (born_date < min_born_date) min_born_date = born_date;
        if (max_death_date < death_date) max_death_date = death_date;
    }

    std::map<Date, int> dates_list = make_dates_map(min_born_date, max_death_date);

    for(int i = 0; i < n; ++i){
        Date start_meet_age = Date(dates_born[i].day(), dates_born[i].month(), dates_born[i].year() + 18);
        Date age_80 = Date(dates_born[i].day(), dates_born[i].month(), dates_born[i].year() + 80);
        Date end_meet_age = age_80 < dates_death[i] ? age_80 : dates_death[i];
        
        add_meet_marks(start_meet_age, end_meet_age, dates_list);
    }

    std::cout << get_map_max(dates_list) << std::endl;

    return 0;
}
