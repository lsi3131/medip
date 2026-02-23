#include <QtCore/qglobal.h>

#if defined(FM_CORE_LIB)
#  define FM_CORE_EXPORT  Q_DECL_EXPORT
# else
#  define FM_CORE_EXPORT  Q_DECL_IMPORT
#endif
