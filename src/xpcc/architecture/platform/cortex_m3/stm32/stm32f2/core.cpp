// coding: utf-8
// ----------------------------------------------------------------------------
/* Copyright (c) 2011, Roboterclub Aachen e.V.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the Roboterclub Aachen e.V. nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ROBOTERCLUB AACHEN E.V. ''AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL ROBOTERCLUB AACHEN E.V. BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * $Id: core.cpp 662 2011-12-06 18:35:25Z georgi-g $
 */
// ----------------------------------------------------------------------------

#include "core.hpp"
#include <xpcc/architecture/platform/cortex_m3.hpp>

bool
xpcc::stm32::Core::Clock::enableHSE(HSEConfig config, uint32_t wait_cycles){
	if (config == HSE_BYPASS){
		RCC->CR |= RCC_CR_HSEBYP|RCC_CR_HSEON;
	}
	else{
		RCC->CR |= RCC_CR_HSEON;
	}

	volatile uint32_t t = wait_cycles;
	while (!(RCC->CR & RCC_CR_HSERDY) && --t){
	}

	return RCC->CR & RCC_CR_HSERDY;
}

void
xpcc::stm32::Core::Clock::enablePll(PLLSource source, uint8_t pllM, uint16_t pllN){
	uint32_t tmp;

	// read reserved values
	tmp = RCC->PLLCFGR & ~(RCC_PLLCFGR_PLLSRC|RCC_PLLCFGR_PLLM|RCC_PLLCFGR_PLLN|RCC_PLLCFGR_PLLP|RCC_PLLCFGR_PLLQ);
	// PLLSRC source for pll and for plli2s
	tmp |= (source == PLL_HSI)?RCC_PLLCFGR_PLLSRC_HSI:RCC_PLLCFGR_PLLSRC_HSE;
	// PLLM (0) = factor is user defined VCO input frequency must be configured to 2MHz
	tmp |= ((uint32_t)pllM)&RCC_PLLCFGR_PLLM;
	// PLLN (6) = factor is user defined
	tmp |= (((uint32_t)pllN)<<6)&RCC_PLLCFGR_PLLN;
#if defined(STM32F4XX)
	// VCO output frequency must be configured to 336MHz
	// PLLP (16) = 0 (factor = 2) for cpu frequency = 168MHz
	// PLLQ (24) = 7 (factor = 7) for 48MHz
	tmp |= (((uint32_t)7)<<24)&RCC_PLLCFGR_PLLQ;
#elif defined(STM32F2XX)
	// VCO output frequency must be configured to 240MHz
	// PLLP (16) = 0 (factor = 2) for cpu frequency = 168MHz
	// PLLQ (24) = 5 (factor = 5) for 48MHz
	tmp |= (((uint32_t)5)<<24)&RCC_PLLCFGR_PLLQ;
	#error "this is not tested yet"
#else
	#error "this file is not supposed to be used with given cpu"
#endif


	RCC->PLLCFGR = tmp;

	// enable pll
	RCC->CR |= RCC_CR_PLLON;
}

bool
xpcc::stm32::Core::Clock::switchToPll(uint32_t wait_cycles){
	volatile uint32_t t = wait_cycles;
	while (!(RCC->CR & RCC_CR_PLLRDY)){
		if (!(--t))
			return false;
	}

#if defined(STM32F4XX)
	// APB2 84MHz, APB1 42MHz, AHB 168MHz, select pll as source
	RCC->CFGR =
			(RCC->CFGR&0xffff0000) | //try to generate a halfword write
			((RCC_CFGR_PPRE1_DIV4 | RCC_CFGR_PPRE2_DIV2 | RCC_CFGR_HPRE_DIV1 | RCC_CFGR_SW_PLL)&0x0000ffff);
#elif defined(STM32F2XX)
	// APB2 60MHz, APB1 30MHz, AHB 120MHz, select pll as source
	RCC->CFGR =
			(RCC->CFGR&0xffff0000) | //try to generate a halfword write
			((RCC_CFGR_PPRE1_DIV4 | RCC_CFGR_PPRE2_DIV2 | RCC_CFGR_HPRE_DIV1 | RCC_CFGR_SW_PLL)&0x0000ffff);
	#error "this is not tested yet"
#else
	#error "this file is not supposed to be used with given cpu"
#endif

	/* Wait till the main PLL is used as system clock source */
	while ((RCC->CFGR & (uint32_t)RCC_CFGR_SWS ) != RCC_CFGR_SWS_PLL);
	{
	}

	return true;
}