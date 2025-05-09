#include <Uefi.h>
#include <Library/UefiLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>

struct MemoryMap {
  UINTN buffer_size;
  VOID* buffer;
  UINTN map_size;
  UINTN map_key;
  UINTN descriptor_size;
  UINT32 descriptor_version;
};

EFI_STATUS GetMemoryMap(struct MemoryMap* map) {
  // if (map->buffer == NULL) {
  //   return EFI_BUFFER_TOO_SMALL;
  // }

  map->map_size = map->buffer_size;
  return gBS->GetMemoryMap(
      &map->map_size,
      (EFI_MEMORY_DESCRIPTOR*)map->buffer,
      &map->map_key,
      &map->descriptor_size,
      &map->descriptor_version);
}

EFI_STATUS EFIAPI UefiMain(IN EFI_HANDLE ImageHandle, IN EFI_SYSTEM_TABLE *SystemTable) {
  // メモリマップを格納するためのバッファを確保（4096バイト×4）
  CHAR8 memmap_buf[4096 * 4];
  
  // メモリマップを格納する構造体の初期化
  // memmap_bufのサイズとポインタを指定してMemoryMap構造体を初期化
  struct MemoryMap memmap = {sizeof(memmap_buf), memmap_buf, 0, 0, 0, 0};
  
  // GetMemoryMapを呼び出して、UEFIのメモリマップ情報をmemmap_bufに取得
  GetMemoryMap(&memmap);

  // 取得したメモリマップのサイズを表示
  Print(L"Memory map size: %lu bytes\n", memmap.map_size);
  
  // 1エントリ（1セグメント）のサイズを表示
  Print(L"descriptor_size: %lu bytes\n", memmap.descriptor_size);
  
  // メモリマップ内のセグメント（エントリ）の個数を表示
  // map_size（メモリマップ全体のサイズ）をdescriptor_size（1エントリのサイズ）で割った値がエントリ数
  Print(L"entry count: %lu\n", memmap.map_size / memmap.descriptor_size);

  // プログラムを無限ループで停止（UEFIのアプリケーションは終了しない）
  while (1);

  return EFI_SUCCESS;
}

