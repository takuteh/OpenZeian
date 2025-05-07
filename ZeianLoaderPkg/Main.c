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
  if (map->buffer == NULL) {
    return EFI_BUFFER_TOO_SMALL;
  }

  map->map_size = map->buffer_size;
  return gBS->GetMemoryMap(
      &map->map_size,
      (EFI_MEMORY_DESCRIPTOR*)map->buffer,
      &map->map_key,
      &map->descriptor_size,
      &map->descriptor_version);
}

EFI_STATUS EFIAPI UefiMain(IN EFI_HANDLE ImageHandle, IN EFI_SYSTEM_TABLE *SystemTable) {
  struct MemoryMap memmap;

  // 最初はNULLを渡して必要なサイズを取得
  // 以下のパラメータを渡すとUEFIが必要なバッファサイズをmap_sizeに格納する
  memmap.buffer_size = 0;
  memmap.buffer = NULL;
  EFI_STATUS status = GetMemoryMap(&memmap);
  //最初はEFI_BUFFER_TOO_SMALLが返ってくるはず
  if (status != EFI_BUFFER_TOO_SMALL) {
    Print(L"Unexpected error: %r\n", status);
    return status;
  }

  // map_size（必要なバッファサイズ）を表示
  Print(L"Memory map size: %lu bytes\n", memmap.map_size);

  while (1); // 停止

  return EFI_SUCCESS;
}
