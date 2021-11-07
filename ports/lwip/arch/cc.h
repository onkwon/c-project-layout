#ifndef LWIP_PORT_ARCH_CC_H
#define LWIP_PORT_ARCH_CC_H

#define PACK_STRUCT_STRUCT		__attribute__((packed))

#define LWIP_PLATFORM_DIAG(x)		do {printf x;} while (0)

#endif /* LWIP_PORT_ARCH_CC_H */
