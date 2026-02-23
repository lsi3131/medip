#include <QtCore/qglobal.h>

#if defined(FM_NET_LIB)
#  define FM_NET_EXPORT  Q_DECL_EXPORT
# else
#  define FM_NET_EXPORT  Q_DECL_IMPORT
#endif
