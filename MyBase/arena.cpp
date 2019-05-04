/*
** Copyright (c) 2019 ZhangQuan
**
** This program is free software; you can redistribute it and/or
** modify it under the terms of the GNU General Public
** License as published by the Free Software Foundation; either
** version 3 of the License, or (at your option) any later version.
**
** Author contact information:
**   zhangqhn@foxmail.com
*/

#include "arena.h"
#include <assert.h>

static const int kBlockSize = 4096;

Arena::Arena()
{
  alloc_ptr_ = nullptr;
  alloc_bytes_remaining_ = 0;
}

Arena::~Arena()
{
  for (size_t i = 0; i < blocks_.size(); i++) {
    delete[] blocks_[i];
  }
}

char* Arena::Allocate(size_t bytes)
{
  assert(bytes > 0);
  if (bytes <= alloc_bytes_remaining_) {
    char* result = alloc_ptr_;
    alloc_ptr_ += bytes;
    alloc_bytes_remaining_ -= bytes;
    return result;
  }

  return AllocateFallback(bytes);
}

char* Arena::AllocateAligned(size_t bytes)
{
  const int align = (sizeof(void*) > 8) ? sizeof(void*) : 8;
  assert((align & (align - 1)) == 0);
  size_t current_mod = reinterpret_cast<uintptr_t>(alloc_ptr_) & (align - 1);
  size_t slop = (current_mod == 0 ? 0 : (align - current_mod));
  size_t needed = bytes + slop;
  char* result;
  if (needed <= alloc_bytes_remaining_) {
    result = alloc_ptr_ + slop;
    alloc_ptr_ += needed;
    alloc_bytes_remaining_ -= needed;
  }
  else {
    result = AllocateFallback(bytes);
  }
  assert((reinterpret_cast<uintptr_t>(result) & (align - 1)) == 0);
  return result;
}

char* Arena::AllocateFallback(size_t bytes)
{
  if (bytes > (kBlockSize / 4))
  {
    char* result = AllocateNewBlock(bytes);
    return result;
  }

  alloc_ptr_ = AllocateNewBlock(kBlockSize);
  alloc_bytes_remaining_ = kBlockSize;

  char* result = alloc_ptr_;
  alloc_ptr_ += bytes;
  alloc_bytes_remaining_ -= bytes;
  return result;
}

char* Arena::AllocateNewBlock(size_t block_bytes)
{
  char* result = new char[block_bytes];
  blocks_.push_back(result);
  return result;
}

