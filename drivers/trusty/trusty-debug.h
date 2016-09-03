#ifndef _TRUSTY_DEBUG_H
#define _TRUSTY_DEBUG_H

/* Debug message event */
#define DBG_EVT_NONE        (0)	/* No event */
#define DBG_EVT_CMD         (1 << 0)	/* SEC CMD related event */
#define DBG_EVT_FUNC        (1 << 1)	/* SEC function event */
#define DBG_EVT_INFO        (1 << 2)	/* SEC information event */
#define DBG_EVT_DBG         (1 << 4)	/* SEC information event */
#define DBG_EVT_WARN        (1 << 30)	/* Warning event */
#define DBG_EVT_ERR         (1 << 31)	/* Error event */
#define DBG_EVT_ALL         (0xffffffff)

#define DBG_EVT_MASK        (DBG_EVT_ALL)

#ifndef LOG_TAG
#error "Please define LOG_TAG for each module before include trusty-debug.h\n"
#endif

#define LOGI(fmt, args...) \
do { \
	if (DBG_EVT_INFO & DBG_EVT_MASK) { \
		printk("[%s] "fmt, LOG_TAG, ##args); \
	} \
} while (0)

#define LOGD(fmt, args...) \
do { \
	if (DBG_EVT_DBG & DBG_EVT_MASK) { \
		printk("[%s] "fmt, LOG_TAG, ##args); \
	} \
} while (0)

#define LOGW(fmt, args...) \
do { \
	if (DBG_EVT_WARN & DBG_EVT_MASK) { \
		printk("[%s] "fmt, LOG_TAG, ##args); \
	} \
} while (0)

#define LOGE(fmt, args...) \
do { \
	if (DBG_EVT_ERR & DBG_EVT_MASK) { \
		printk("[%s] "fmt, LOG_TAG, ##args); \
	} \
} while (0)

#endif /* END _TRUSTY_DEBUG_H */
