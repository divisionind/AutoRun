/*
 * Copyright (C) 2020, Andrew Howard, <divisionind.com>
 * All rights reserved.
 */

#ifndef AUTORUN2_VKARRAY_H
#define AUTORUN2_VKARRAY_H

#include <cstdint>

/**
 * Prepares an array of keycodes shows status should be monitored by this program.
 * @param arraySize
 * @return
 */
uint8_t* prepare_vkarray(int* arraySize);

#endif //AUTORUN2_VKARRAY_H
