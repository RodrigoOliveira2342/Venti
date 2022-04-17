/*
 * Copyright (c) 2019 Peter Bigot Consulting, LLC
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/* Sample which uses the filesystem API with littlefs */
#include "tablesFS.h"

/* Matches LFS_NAME_MAX */
#define MAX_PATH_LEN  255 //255

#define PARTITION_NODE DT_NODELABEL(lfs1)

#if DT_NODE_EXISTS(PARTITION_NODE)
FS_FSTAB_DECLARE_ENTRY(PARTITION_NODE);
#else /* PARTITION_NODE */
FS_LITTLEFS_DECLARE_DEFAULT_CONFIG(storage);
static struct fs_mount_t lfs_storage_mnt = {
	.type = FS_LITTLEFS,
	.fs_data = &storage,
	.storage_dev = (void *)FLASH_AREA_ID(storage),
	.mnt_point = "/lfs",
};
#endif /* PARTITION_NODE */


struct fs_mount_t *fREF;

char fnameLPS[MAX_PATH_LEN];
char fnameHSC[MAX_PATH_LEN];
char fnameSDP[MAX_PATH_LEN];
char fnameFIO2[MAX_PATH_LEN];

int flagfiles[4]={0,0,0,0};

struct fs_file_t fLPS;
struct fs_file_t fHSC;
struct fs_file_t fSDP;
struct fs_file_t fFIO2;

struct fs_dirent dirent;

float tabela_HSC[43]={0}; 
float tabela_SDP[43]={0}; 
float tabela_FIO2[2]={0};
float offset_LPS[1] = {0};

void ConfigureLFS(void)
{
	struct fs_mount_t *mp =
#if DT_NODE_EXISTS(PARTITION_NODE)
		&FS_FSTAB_ENTRY(PARTITION_NODE)
#else
		&lfs_storage_mnt
#endif
		;
	unsigned int id = (uintptr_t)mp->storage_dev;

	struct fs_statvfs sbuf;
	const struct flash_area *pfa;
	int rc;

	snprintf(fnameLPS, sizeof(fnameLPS), "%s/offsetLPS", mp->mnt_point);
	snprintf(fnameHSC, sizeof(fnameHSC), "%s/tableHSC", mp->mnt_point);
	snprintf(fnameSDP, sizeof(fnameSDP), "%s/tableSDP", mp->mnt_point);
	snprintf(fnameFIO2, sizeof(fnameFIO2), "%s/tableFIO2", mp->mnt_point);

	rc = flash_area_open(id, &pfa);
	if (rc < 0)return;
	/* Optional wipe flash contents */
	if (IS_ENABLED(CONFIG_APP_WIPE_STORAGE)) {
		rc = flash_area_erase(pfa, 0, pfa->fa_size);
	}

	flash_area_close(pfa);

	/* Do not mount if auto-mount has been enabled */
#if !DT_NODE_EXISTS(PARTITION_NODE) ||						\
	!(FSTAB_ENTRY_DT_MOUNT_FLAGS(PARTITION_NODE) & FS_MOUNT_FLAG_AUTOMOUNT)
	rc = fs_mount(mp);
	if (rc < 0) {
		// printk("FAIL: mount id %" PRIuPTR " at %s: %d\n",
		//        (uintptr_t)mp->storage_dev, mp->mnt_point, rc);
		return;
	}
	// printk("%s mount: %d\n", mp->mnt_point, rc);
#else
	// printk("%s automounted\n", mp->mnt_point);
#endif

	rc = fs_statvfs(mp->mnt_point, &sbuf);
	fREF = mp;

	fs_file_t_init(&fLPS);

	flagfiles[0] = fs_stat(fnameLPS, &dirent);

	fs_file_t_init(&fHSC);

	flagfiles[1] = fs_stat(fnameHSC, &dirent);
	
	fs_file_t_init(&fSDP);

	flagfiles[2] = fs_stat(fnameSDP, &dirent);
	
	
	fs_file_t_init(&fFIO2);

	flagfiles[3] = fs_stat(fnameFIO2, &dirent);

}

void Savedata(int tipo){

	struct fs_file_t *file;
	char *fname;
	int len = 0;
	float *data;
	int rc;
	
	switch(tipo){
		case 0:
			file = &fLPS;
			fname = fnameLPS;
			len = 1;
			data =  offset_LPS;
			break;
		case 1:
			file = &fHSC;
			fname = fnameHSC;
			len = 43;
			data = tabela_HSC;
			break;
		case 2:
			file = &fSDP;
			fname = fnameSDP;
			len = 43;
			data = tabela_SDP;
			break;
		case 3:
			file = &fFIO2;
			fname = fnameFIO2;
			len = 2;
			data = tabela_FIO2;
			break;
		default:
			return;
	}
	rc = fs_open(file, fname, FS_O_CREATE | FS_O_RDWR);
	for(int i=0;i<len;i++){
			rc = fs_write(file, &data[i], sizeof(float));
		}
	rc = fs_seek(file, 0, FS_SEEK_SET);
	rc = fs_close(file);


}	

void Readdata(int tipo){
	struct fs_file_t *file;
	char *fname;
	int len = 0;
	float *data;
	int rc;
	switch(tipo){
		case 0:
			file = &fLPS;
			fname = fnameLPS;
			len = 1;
			data =  offset_LPS;
			break;
		case 1:
			file = &fHSC;
			fname = fnameHSC;
			len = 43;
			data = tabela_HSC;
			break;
		case 2:
			file = &fSDP;
			fname = fnameSDP;
			len = 43;
			data = tabela_SDP;
			break;
		case 3:
			file = &fFIO2;
			fname = fnameFIO2;
			len = 2;
			data = tabela_FIO2;
			break;
		default:
			return;
	}

	rc = fs_open(file, fname, FS_O_CREATE | FS_O_RDWR);

	if (rc >= 0) {
		for(int i=0;i<len;i++){
			rc = fs_read(file, &data[i], sizeof(float));
		}
	}else{
		for(int i=0;i<len;i++){
			rc = fs_write(file, &data[i], sizeof(float));
		}
	}
	
	rc = fs_seek(file, 0, FS_SEEK_SET);
	rc = fs_close(file);

}


// uint32_t CFSTEST2(){
	
// 	// uint32_t lFSC =  CFSTEST2();
// 	// uint8_t lFSCV[4];
	
// 	// memcpy(lFSCV,&lFSC,4); 
// 	// SendMsg(lFSCV,4);

// 		readdata(0);
// 		offset_LPS[0]++;
// 		savedata(0);

// 		readdata(2);
// 		tabela_SDP[1]+=1.5;
// 		savedata(2);
// 		return  (int)(tabela_SDP[1]*10);
// }



