#define VERSION_MAJOR            1
#define VERSION_MINOR            0
#define VERSION_REVISION         4
#define VERSION_BUILDNUM         0
#define VERSION_GCID             0xec009ca2
#define CUSTOMER_NAME            unknown
#define CN_1                     'u'
#define CN_2                     'n'
#define CN_3                     'k'
#define CN_4                     'n'
#define CN_5                     'o'
#define CN_6                     'w'
#define CN_7                     'n'
#define CN_8                     '#'
#define BUILDING_TIME            "Wed Feb 20 09:18:06 2019"
#define NAME2STR(a)              #a
#define CUSTOMER_NAME_S          #NAME2STR(CUSTOMER_NAME)
#define NUM4STR(a,b,c,d)         #a "." #b "." #c "." #d
#define VERSIONBUILDSTR(a,b,c,d) NUM4STR(a,b,c,d)
#define VERSION_BUILD_STR        VERSIONBUILDSTR(VERSION_MAJOR,VERSION_MINOR,VERSION_REVISION,VERSION_BUILDNUM)
