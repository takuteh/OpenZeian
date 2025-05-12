#include  <Uefi.h>
#include  <Library/UefiLib.h>
#include  <Library/UefiBootServicesTableLib.h>
#include  <Library/PrintLib.h>
#include  <Protocol/LoadedImage.h>
#include  <Protocol/SimpleFileSystem.h>
#include  <Protocol/DiskIo2.h>
#include  <Protocol/BlockIo.h>
#include  <Guid/FileInfo.h>

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
const CHAR16* GetMemoryTypeUnicode(EFI_MEMORY_TYPE type) {
  switch (type) {
    case EfiReservedMemoryType: return L"EfiReservedMemoryType";
    case EfiLoaderCode: return L"EfiLoaderCode";
    case EfiLoaderData: return L"EfiLoaderData";
    case EfiBootServicesCode: return L"EfiBootServicesCode";
    case EfiBootServicesData: return L"EfiBootServicesData";
    case EfiRuntimeServicesCode: return L"EfiRuntimeServicesCode";
    case EfiRuntimeServicesData: return L"EfiRuntimeServicesData";
    case EfiConventionalMemory: return L"EfiConventionalMemory";
    case EfiUnusableMemory: return L"EfiUnusableMemory";
    case EfiACPIReclaimMemory: return L"EfiACPIReclaimMemory";
    case EfiACPIMemoryNVS: return L"EfiACPIMemoryNVS";
    case EfiMemoryMappedIO: return L"EfiMemoryMappedIO";
    case EfiMemoryMappedIOPortSpace: return L"EfiMemoryMappedIOPortSpace";
    case EfiPalCode: return L"EfiPalCode";
    case EfiPersistentMemory: return L"EfiPersistentMemory";
    case EfiMaxMemoryType: return L"EfiMaxMemoryType";
    default: return L"InvalidMemoryType";
  }
}

// EFIのメモリマップ情報をfileに保存する関数
EFI_STATUS SaveMemoryMap(struct MemoryMap* map, EFI_FILE_PROTOCOL* file) {
  CHAR8 buf[256];  // 出力用の一時バッファ（ASCII文字列）
  UINTN len;       // 書き込みサイズ

  // ヘッダ行の文字列（列名）を定義
  CHAR8* header =
    "Index, Type, Type(name), PhysicalStart, NumberOfPages, Attribute\n";
  len = AsciiStrLen(header);         // ヘッダの文字列長を取得
  file->Write(file, &len, header);   // ファイルにヘッダを書き込む

  // メモリマップの先頭アドレスとサイズを表示（デバッグ用）
  Print(L"map->buffer = %08lx, map->map_size = %08lx\n",
      map->buffer, map->map_size);

  EFI_PHYSICAL_ADDRESS iter;  // メモリマップの現在位置を指すアドレス
  int i;                      // インデックスカウンタ

  // メモリマップを1エントリずつ処理
  for (iter = (EFI_PHYSICAL_ADDRESS)map->buffer, i = 0;
       iter < (EFI_PHYSICAL_ADDRESS)map->buffer + map->map_size;
       iter += map->descriptor_size, i++) {

    // 現在の位置のメモリ記述子を取得
    EFI_MEMORY_DESCRIPTOR* desc = (EFI_MEMORY_DESCRIPTOR*)iter;

    // 記述子の情報をフォーマットしてbufに書き込む
    len = AsciiSPrint(
        buf, sizeof(buf),
        "%u, %x, %-ls, %08lx, %lx, %lx\n",         // CSV形式
        i,                                         // インデックス
        desc->Type,                                // メモリタイプ（数値）
        GetMemoryTypeUnicode(desc->Type),          // メモリタイプ（名前）
        desc->PhysicalStart,                       // 物理開始アドレス
        desc->NumberOfPages,                       // ページ数
        desc->Attribute & 0xffffflu);              // 属性（下位64bit）

    // フォーマットした文字列をファイルに書き込む
    file->Write(file, &len, buf);
  }

  // 正常終了を返す
  return EFI_SUCCESS;
}


EFI_STATUS OpenRootDir(EFI_HANDLE image_handle, EFI_FILE_PROTOCOL** root) {
  EFI_LOADED_IMAGE_PROTOCOL* loaded_image;
  EFI_SIMPLE_FILE_SYSTEM_PROTOCOL* fs;

  // 実行中のこのUEFIアプリケーションが格納されているデバイス情報を取得
  gBS->OpenProtocol(
      image_handle,
      &gEfiLoadedImageProtocolGuid,
      (VOID**)&loaded_image,
      image_handle,
      NULL,
      EFI_OPEN_PROTOCOL_BY_HANDLE_PROTOCOL);

  // そのデバイスが使っているファイルシステムの操作インターフェースを取得
  gBS->OpenProtocol(
      loaded_image->DeviceHandle,
      &gEfiSimpleFileSystemProtocolGuid,
      (VOID**)&fs,
      image_handle,
      NULL,
      EFI_OPEN_PROTOCOL_BY_HANDLE_PROTOCOL);

  // ファイルシステムからルートディレクトリ（"/"）を開いて使えるようにする
  fs->OpenVolume(fs, root);

  return EFI_SUCCESS;
}

EFI_STATUS EFIAPI UefiMain(EFI_HANDLE image_handle, EFI_SYSTEM_TABLE *system_table) {
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
  
  // ルートディレクトリの操作インターフェースの取得
  EFI_FILE_PROTOCOL* root_dir;
  OpenRootDir(image_handle, &root_dir);

  // memmapというファイルを開いて操作インターフェースを取得
  EFI_FILE_PROTOCOL* memmap_file;
  root_dir->Open(
      root_dir, &memmap_file, L"\\memmap",
      EFI_FILE_MODE_READ | EFI_FILE_MODE_WRITE | EFI_FILE_MODE_CREATE, 0);

  SaveMemoryMap(&memmap, memmap_file);

  //操作インターフェースに用意されているClose関数でファイルを閉じる
  memmap_file->Close(memmap_file);

  EFI_FILE_PROTOCOL* kernel_file;
  //カーネルファイルをオープン
  root_dir->Open(
      root_dir, &kernel_file, L"\\kernel.elf",
      EFI_FILE_MODE_READ, 0);

  // ファイル情報を格納するためのバッファサイズを指定
  // - `EFI_FILE_INFO` 構造体のサイズに加えてファイル名を格納できるサイズを足している
  UINTN file_info_size = sizeof(EFI_FILE_INFO) + sizeof(CHAR16) * 12;
  // 実際にファイル情報を読み取る（展開する）ためのバッファを定義（スタック上）
  UINT8 file_info_buffer[file_info_size];
  //取得したファイル情報をバッファに書き込む
  kernel_file->GetInfo(
      kernel_file, &gEfiFileInfoGuid,
      &file_info_size, file_info_buffer);

  EFI_FILE_INFO* file_info = (EFI_FILE_INFO*)file_info_buffer;

  //カーネルのサイズを取得
  UINTN kernel_file_size = file_info->FileSize;

  //カーネルの読み込み先アドレスを指定
  EFI_PHYSICAL_ADDRESS kernel_base_addr = 0x100000;
  //カーネルサイズ分のメモリを確保
  gBS->AllocatePages(
      AllocateAddress, EfiLoaderData,
      (kernel_file_size + 0xfff) / 0x1000, &kernel_base_addr);

  //カーネルをメモリにロード
  kernel_file->Read(kernel_file, &kernel_file_size, (VOID*)kernel_base_addr);
  Print(L"Kernel: 0x%0lx (%lu bytes)\n", kernel_base_addr, kernel_file_size);
  // プログラムを無限ループで停止（UEFIのアプリケーションは終了しない）
  while (1);

  return EFI_SUCCESS;
}

