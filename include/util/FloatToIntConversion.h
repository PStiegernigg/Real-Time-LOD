/* 
 * File:   FloatToIntConversion.h
 * Author: Philipp Stiegernigg <p.stiegernigg@student.uibk.ac.at>
 *
 * Created on July 25, 2015, 3:10 PM
 */

#ifndef FLOATTOINTCONVERSION_H
#define	FLOATTOINTCONVERSION_H

uint32_t FloatFlip(float value)
{       
        uint32_t f = *reinterpret_cast<float*>(&value);
	uint32_t mask = -int32_t(f >> 31) | 0x80000000;
	return f ^ mask;
}

uint32_t InvertetFloatFlip(uint32_t f)
{
	uint32_t mask = ((f >> 31) - 1) | 0x80000000;
	return f ^ mask;
}

#endif	/* FLOATTOINTCONVERSION_H */

