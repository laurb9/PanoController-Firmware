
#if !defined(GIT_VERSION)
#define GIT_VERSION __DATE__
#endif

#if !defined(ARDUINO_VARIANT)
#if defined(CORE_TEENSY)
#define ARDUINO_VARIANT "teensy"
#define ARDUINO_BOARD "teensy board"
#else
#define ARDUINO_VARIANT "undefined"
#endif
#endif

#if !defined(ARDUINO_BOARD)
#define ARDUINO_BOARD "undefined board"
#endif

#if !defined(PANO_PLATFORM)
#if defined(PLATFORM_PANOCONTROLLER_V4)
#define PANO_PLATFORM "PanoController V4"
#elif defined(PLATFORM_GIGAPAN)
#define PANO_PLATFORM "Gigapan Epic 100"
#elif defined(PLATFORM_MECHA_E1)
#define PANO_PLATFORM "Nodal Ninja MECHA E1"
#endif
#endif
