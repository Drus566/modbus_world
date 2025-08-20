#ifndef MB_MODBUS_TRANS_H
#define MB_MODBUS_TRANS_H

#include "ModbusEnums.h"
#include "ModbusRegister.h"

#include <math.h>
#include <cstring>

#define BYTE_LENGTH 8

namespace mb {
namespace types {

class ModbusTrans {
public:
    static void setPrecision(void* val, RegDataType type, int precision) {
        if (type == RegDataType::FLOAT32) setPrecisionFloat32(static_cast<float*>(val), precision);
        else if (type == RegDataType::FLOAT16) setPrecisionFloat16(static_cast<int16_t*>(val), precision);
    }
    
    // TODO: проверить с нулем
    static void setPrecisionFloat16ForWrite(float& val, int precision) {
        val *= std::pow(10, precision);
    }

    static void setPrecisionFloat16(int16_t* val, int precision) {
        int temp = static_cast<int16_t>(*val);
        *val = static_cast<float>(temp / std::pow(10, precision));
    }

    static void setPrecisionFloat32(float* val, int precision) {
        auto prcsn = std::pow(10, precision);
        *val = round(*val * prcsn) / prcsn;
    }

    template <typename T>
    static void interpretReg16(T& result, uint16_t reg, Register& r) {
      	T val = 0;

		RegDataType& data_type = r.reg_info.data_type;
        int precision = r.reg_info.precision;

      	if (data_type == RegDataType::UINT16) val = static_cast<unsigned short>(reg);
      	else if (data_type == RegDataType::INT16) val = static_cast<short>(reg);
      	else if (data_type == RegDataType::FLOAT16) {
        	int temp = static_cast<short>(reg);
        	val = static_cast<float>(temp / std::pow(10, precision));
      	}

		result = val;
    }

	template <typename T>
    static void interpretReg16(T& result, uint16_t reg, const RegDataType data_type, const char precision = 0) {
      	T val = 0;
		
      	if (data_type == RegDataType::UINT16) val = static_cast<unsigned short>(reg);
      	else if (data_type == RegDataType::INT16) val = static_cast<short>(reg);
      	else if (data_type == RegDataType::FLOAT16) {
        	int temp = static_cast<short>(reg);
        	val = static_cast<float>(temp / std::pow(10, precision));
      	}

		result = val;
    }

    template <typename T>
    static void interpretReg32(T& result, uint16_t regs[2], Register& r) {
		T val = 0;

        RegDataType& data_type = r.reg_info.data_type;
        RegDataOrder& data_order = r.reg_info.order;
        int precision = r.reg_info.precision;
        
      	if (data_type == RegDataType::INT32) {          
        	if (data_order == RegDataOrder::AB_CD)      val = modbusGetInt32Abcd(&regs[0]);
        	else if (data_order == RegDataOrder::BA_DC) val = modbusGetInt32Badc(&regs[0]);
        	else if (data_order == RegDataOrder::CD_AB) val = modbusGetInt32Cdab(&regs[0]);
        	else if (data_order == RegDataOrder::DC_BA) val = modbusGetInt32Dcba(&regs[0]);
		}
        else if (data_type == RegDataType::UINT32) {          
        	if (data_order == RegDataOrder::AB_CD)      val = modbusGetUint32Abcd(&regs[0]);
        	else if (data_order == RegDataOrder::BA_DC) val = modbusGetUint32Badc(&regs[0]);
        	else if (data_order == RegDataOrder::CD_AB) val = modbusGetUint32Cdab(&regs[0]);
        	else if (data_order == RegDataOrder::DC_BA) val = modbusGetUint32Dcba(&regs[0]);
		}
      	else if (data_type == RegDataType::FLOAT32) {
      		if (data_order == RegDataOrder::AB_CD)      val = modbusGetFloatAbcd(&regs[0]);
      		else if (data_order == RegDataOrder::BA_DC) val = modbusGetFloatBadc(&regs[0]);
      		else if (data_order == RegDataOrder::CD_AB) val = modbusGetFloatCdab(&regs[0]);
      		else if (data_order == RegDataOrder::DC_BA) val = modbusGetFloatDcba(&regs[0]);
			auto prcsn = std::pow(10, precision);
			val = round(val * prcsn) / prcsn;
      	}
		result = val;
	}

    template <typename T>
    static void interpretReg32(T& result, uint16_t regs[2], const RegDataType data_type, const RegDataOrder data_order, const char precision = 0) {
		T val = 0;

      	if (data_type == RegDataType::INT32) {          
        	if (data_order == RegDataOrder::AB_CD)      val = modbusGetInt32Abcd(&regs[0]);
        	else if (data_order == RegDataOrder::BA_DC) val = modbusGetInt32Badc(&regs[0]);
        	else if (data_order == RegDataOrder::CD_AB) val = modbusGetInt32Cdab(&regs[0]);
        	else if (data_order == RegDataOrder::DC_BA) val = modbusGetInt32Dcba(&regs[0]);
		}
        else if (data_type == RegDataType::UINT32) {          
        	if (data_order == RegDataOrder::AB_CD)      val = modbusGetUint32Abcd(&regs[0]);
        	else if (data_order == RegDataOrder::BA_DC) val = modbusGetUint32Badc(&regs[0]);
        	else if (data_order == RegDataOrder::CD_AB) val = modbusGetUint32Cdab(&regs[0]);
        	else if (data_order == RegDataOrder::DC_BA) val = modbusGetUint32Dcba(&regs[0]);
		}
      	else if (data_type == RegDataType::FLOAT32) {
      		if (data_order == RegDataOrder::AB_CD)      val = modbusGetFloatAbcd(&regs[0]);
      		else if (data_order == RegDataOrder::BA_DC) val = modbusGetFloatBadc(&regs[0]);
      		else if (data_order == RegDataOrder::CD_AB) val = modbusGetFloatCdab(&regs[0]);
      		else if (data_order == RegDataOrder::DC_BA) val = modbusGetFloatDcba(&regs[0]);
			auto prcsn = std::pow(10, precision);
			val = round(val * prcsn) / prcsn;
      	}
		result = val;
	}

    static uint32_t getSwapBytes(uint32_t* src, const RegDataOrder data_order) {
        uint32_t result = *src;
        swapBytes(&result, data_order);
        return result;
    }

    static void swapBytes(uint32_t* src, const RegDataOrder data_order) {
        if (data_order == RegDataOrder::AB_CD)      swapToAbcd(src);
      	else if (data_order == RegDataOrder::BA_DC) swapToBadc(src);
      	else if (data_order == RegDataOrder::CD_AB) swapToCdab(src);
      	else if (data_order == RegDataOrder::DC_BA) swapToDcba(src);
    }

    static float modbusGetFloatAbcd(const uint16_t *src) { 
        uint32_t response = modbusGetAbcd(src);
        return static_cast<float>(*((float*)(&response))); 
    }
    static float modbusGetFloatDcba(const uint16_t *src) { 
        uint32_t response = modbusGetDcba(src);
        return static_cast<float>(*((float*)(&response))); 
    }
    static float modbusGetFloatBadc(const uint16_t *src) { 
        uint32_t response = modbusGetBadc(src);
        return static_cast<float>(*((float*)(&response))); 
    }
    static float modbusGetFloatCdab(const uint16_t *src) { 
        uint32_t response = modbusGetCdab(src);
        return static_cast<float>(*((float*)(&response))); 
    }
    static float modbusGetFloat(const uint16_t *src) { 
        uint32_t response = modbusGetCdab(src);
        return static_cast<float>(*((float*)(&response))); 
    }
 
    static int modbusGetInt32Abcd(const uint16_t *src) { return static_cast<int>(modbusGetAbcd(src)); }
    static int modbusGetInt32Dcba(const uint16_t *src) { return static_cast<int>(modbusGetDcba(src)); }
    static int modbusGetInt32Badc(const uint16_t *src) { return static_cast<int>(modbusGetBadc(src)); }
    static int modbusGetInt32Cdab(const uint16_t *src) { return static_cast<int>(modbusGetCdab(src)); }
    static int modbusGetInt32(const uint16_t *src)     { return static_cast<int>(modbusGetCdab(src)); }
 
    static unsigned int modbusGetUint32Abcd(const uint16_t *src) { return static_cast<unsigned int>(modbusGetAbcd(src)); }
    static unsigned int modbusGetUint32Dcba(const uint16_t *src) { return static_cast<unsigned int>(modbusGetDcba(src)); }
    static unsigned int modbusGetUint32Badc(const uint16_t *src) { return static_cast<unsigned int>(modbusGetBadc(src)); }
    static unsigned int modbusGetUint32Cdab(const uint16_t *src) { return static_cast<unsigned int>(modbusGetCdab(src)); }
    static unsigned int modbusGetUint32(const uint16_t *src)     { return static_cast<unsigned int>(modbusGetCdab(src)); }
    
    template <typename T>
    static void modbusSetByteOrder(T val, uint16_t* dest, RegDataOrder data_order) {
        if (data_order == RegDataOrder::AB_CD)      modbusSetAbcd(dest);
      	else if (data_order == RegDataOrder::BA_DC) modbusSetBadc(dest);
      	else if (data_order == RegDataOrder::CD_AB) modbusSetCdab(dest);
      	else if (data_order == RegDataOrder::DC_BA) modbusSetDcba(dest);
    }

    /* Set to 4 bytes for Modbus w/o any conversion (ABCD) */
	template <typename T>
    static void modbusSetAbcd(T val, uint16_t *dest) {
        T * ptr = &val;
        uint32_t * iptr = (uint32_t *)ptr;
        uint32_t i = *iptr;
        uint8_t a, b, c, d;
    
        a = (i >> 24) & 0xFF;
        b = (i >> 16) & 0xFF;
        c = (i >> 8) & 0xFF;
        d = (i >> 0) & 0xFF;
    
        dest[0] = (a << 8) | b;
        dest[1] = (c << 8) | d;
    }
    
    /* Set to 4 bytes for Modbus with byte and word swap conversion (DCBA) */
    template <typename T>
    static void modbusSetDcba(T val, uint16_t *dest) {
        T * ptr = &val;
        uint32_t * iptr = (uint32_t *)ptr;
        uint32_t i = *iptr;
        uint8_t a, b, c, d;
    
        a = (i >> 24) & 0xFF;
        b = (i >> 16) & 0xFF;
        c = (i >> 8) & 0xFF;
        d = (i >> 0) & 0xFF;
    
        dest[0] = (d << 8) | c;
        dest[1] = (b << 8) | a;
    }
    
    /* Set to 4 bytes for Modbus with byte swap conversion (BADC) */
    template <typename T>
    static void modbusSetBadc(T val, uint16_t *dest) {
        T * ptr = &val;
        uint32_t * iptr = (uint32_t *)ptr;
        uint32_t i = *iptr;
        uint8_t a, b, c, d;
    
        a = (i >> 24) & 0xFF;
        b = (i >> 16) & 0xFF;
        c = (i >> 8) & 0xFF;
        d = (i >> 0) & 0xFF;
    
        dest[0] = (b << 8) | a;
        dest[1] = (d << 8) | c;
    }
    
    /* Set to 4 bytes for Modbus with word swap conversion (CDAB) */
    template <typename T>
    static void modbusSetCdab(T val, uint16_t *dest) {
        T * ptr = &val;
        uint32_t * iptr = (uint32_t *)ptr;
        uint32_t i = *iptr;
        uint8_t a, b, c, d;
    
        a = (i >> 24) & 0xFF;
        b = (i >> 16) & 0xFF;
        c = (i >> 8) & 0xFF;
        d = (i >> 0) & 0xFF;
    
        dest[0] = (c << 8) | d;
        dest[1] = (a << 8) | b;
    }

    static void modbusSetAbcd(uint16_t *dest) {
        uint32_t *iptr = (uint32_t *)dest;
        uint32_t i = *iptr;
        uint8_t a, b, c, d;
    
        a = (i >> 24) & 0xFF;
        b = (i >> 16) & 0xFF;
        c = (i >> 8) & 0xFF;
        d = (i >> 0) & 0xFF;
    
        dest[0] = (a << 8) | b;
        dest[1] = (c << 8) | d;
    }
    
    static void modbusSetDcba(uint16_t *dest) {
        uint32_t *iptr = (uint32_t *)dest;
        uint32_t i = *iptr;
        uint8_t a, b, c, d;
    
        a = (i >> 24) & 0xFF;
        b = (i >> 16) & 0xFF;
        c = (i >> 8) & 0xFF;
        d = (i >> 0) & 0xFF;
    
        dest[0] = (d << 8) | c;
        dest[1] = (b << 8) | a;
    }
    
    static void modbusSetBadc(uint16_t *dest) {
        uint32_t *iptr = (uint32_t *)dest;
        uint32_t i = *iptr;
        uint8_t a, b, c, d;
    
        a = (i >> 24) & 0xFF;
        b = (i >> 16) & 0xFF;
        c = (i >> 8) & 0xFF;
        d = (i >> 0) & 0xFF;
    
        dest[0] = (b << 8) | a;
        dest[1] = (d << 8) | c;
    }
    
    static void modbusSetCdab(uint16_t *dest) {
        uint32_t *iptr = (uint32_t *)dest;
        uint32_t i = *iptr;
        uint8_t a, b, c, d;
    
        a = (i >> 24) & 0xFF;
        b = (i >> 16) & 0xFF;
        c = (i >> 8) & 0xFF;
        d = (i >> 0) & 0xFF;
    
        dest[0] = (c << 8) | d;
        dest[1] = (a << 8) | b;
    }


    /* Get from 4 bytes (Modbus) without any conversion (ABCD) */
    static uint32_t modbusGetAbcd(const uint16_t *src) {
        uint32_t res;
        uint32_t i;
        uint8_t a, b, c, d;

        // Mind: src contains 16-bit numbers in processor-endianness, hence
        //       we use shift operations and do not access memory directly
        a = (src[0] >> 8) & 0xFF; // high byte of first word
        b = (src[0] >> 0) & 0xFF; // low byte of first word
        c = (src[1] >> 8) & 0xFF; // high byte of second word
        d = (src[1] >> 0) & 0xFF; // low byte of second word

        // we assemble 32bit integer always in abcd order via shift operations
        i = (a << 24) | (b << 16) | (c << 8) | (d << 0);
        memcpy(&res, &i, 4);

        return res;
    }

    /* Get from 4 bytes (Modbus) in inversed format (DCBA) */
    static uint32_t modbusGetDcba(const uint16_t *src) {
        uint32_t res;
        uint32_t i;
        uint8_t a, b, c, d;

        // byte order is defined when reading from src: dcba
        d = (src[0] >> 8) & 0xFF;
        c = (src[0] >> 0) & 0xFF;
        b = (src[1] >> 8) & 0xFF;
        a = (src[1] >> 0) & 0xFF;

        // we assemble 32bit integer always in abcd order via shift operations
        i = (a << 24) | (b << 16) | (c << 8) | (d << 0);
        memcpy(&res, &i, 4);

        return res;
    }

    /* Get from 4 bytes (Modbus) with swapped bytes (BADC) */
    static uint32_t modbusGetBadc(const uint16_t *src) {
        uint32_t res;
        uint32_t i;
        uint8_t a, b, c, d;

        // byte order is defined when reading from src: badc
        b = (src[0] >> 8) & 0xFF;
        a = (src[0] >> 0) & 0xFF;
        d = (src[1] >> 8) & 0xFF;
        c = (src[1] >> 0) & 0xFF;

        // we assemble 32bit integer always in abcd order via shift operations
        i = (a << 24) | (b << 16) | (c << 8) | (d << 0);
        memcpy(&res, &i, 4);

        return res;
    }

    /* Get from 4 bytes (Modbus) with swapped words (CDAB) */
    static uint32_t modbusGetCdab(const uint16_t *src) {
        uint32_t res;
        uint32_t i;
        uint8_t a, b, c, d;

        // byte order is defined when reading from src: cdab
        c = (src[0] >> 8) & 0xFF;
        d = (src[0] >> 0) & 0xFF;
        a = (src[1] >> 8) & 0xFF;
        b = (src[1] >> 0) & 0xFF;

        // we assemble 32bit integer always in abcd order via shift operations
        i = (a << 24) | (b << 16) | (c << 8) | (d << 0);
        memcpy(&res, &i, 4);

        return res;
    }

    /* Get from 4 bytes (Modbus) without any conversion (ABCD) */
    static void swapToAbcd(uint32_t *src) {
        uint16_t* ptr = reinterpret_cast<uint16_t*>(src);
        uint8_t a, b, c, d;

        // Mind: src contains 16-bit numbers in processor-endianness, hence
        //       we use shift operations and do not access memory directly
        a = (ptr[0] >> 8) & 0xFF; // high byte of first word
        b = (ptr[0] >> 0) & 0xFF; // low byte of first word
        c = (ptr[1] >> 8) & 0xFF; // high byte of second word
        d = (ptr[1] >> 0) & 0xFF; // low byte of second word

        // we assemble 32bit integer always in abcd order via shift operations
        *src = (a << 24) | (b << 16) | (c << 8) | (d << 0);
    }

    /* Get from 4 bytes (Modbus) in inversed format (DCBA) */
    static void swapToDcba(uint32_t *src) {
        uint16_t* ptr = reinterpret_cast<uint16_t*>(src);
        uint8_t a, b, c, d;

        // byte order is defined when reading from src: dcba
        d = (ptr[0] >> 8) & 0xFF;
        c = (ptr[0] >> 0) & 0xFF;
        b = (ptr[1] >> 8) & 0xFF;
        a = (ptr[1] >> 0) & 0xFF;

        // we assemble 32bit integer always in abcd order via shift operations
        *src = (a << 24) | (b << 16) | (c << 8) | (d << 0);
    }

    /* Get from 4 bytes (Modbus) with swapped bytes (BADC) */
    static void swapToBadc(uint32_t *src) {
        uint16_t* ptr = reinterpret_cast<uint16_t*>(src);
        uint8_t a, b, c, d;

        // byte order is defined when reading from src: badc
        b = (ptr[0] >> 8) & 0xFF;
        a = (ptr[0] >> 0) & 0xFF;
        d = (ptr[1] >> 8) & 0xFF;
        c = (ptr[1] >> 0) & 0xFF;

        // we assemble 32bit integer always in abcd order via shift operations
        *src = (a << 24) | (b << 16) | (c << 8) | (d << 0);
    }

    /* Get from 4 bytes (Modbus) with swapped words (CDAB) */
    static void swapToCdab(uint32_t *src) {
        uint16_t* ptr = reinterpret_cast<uint16_t*>(src);
        uint8_t a, b, c, d;

        // byte order is defined when reading from src: cdab
        c = (ptr[0] >> 8) & 0xFF;
        d = (ptr[0] >> 0) & 0xFF;
        a = (ptr[1] >> 8) & 0xFF;
        b = (ptr[1] >> 0) & 0xFF;

        // we assemble 32bit integer always in abcd order via shift operations
        *src = (a << 24) | (b << 16) | (c << 8) | (d << 0);
    }
};

} // types
} // mb

#endif // MB_MODBUS_TRANS_H