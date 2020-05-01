# SoalShiftSISOP20_modul4_B08

Soal Shift Modul 4 yang berfokus pada penggunaan fuse.

## SOAL 1
Pada soal 1 ini kita diminta melakukan enkripsi versi 1 :
* Jika sebuah direktori dibuat dengan awalan “encv1_”, maka direktori tersebut akan menjadi direktori terenkripsi menggunakan metode 
  enkripsi v1.
* Jika sebuah direktori di-rename dengan awalan “encv1_”, maka direktori tersebut akan menjadi direktori terenkripsi menggunakan metode
  enkripsi v1.
* Apabila sebuah direktori terenkripsi di-rename menjadi tidak terenkripsi, maka isi adirektori tersebut akan terdekrip.
* Setiap pembuatan direktori terenkripsi baru (mkdir ataupun rename) akan tercatat ke sebuah database/log berupa file.
* Semua file yang berada dalam direktori ter enkripsi menggunakan caesar cipher dengan key.

  Key :
  ``` 
  9(ku@AW1[Lmvgax6q`5Y2Ry?+sF!^HKQiBXCUSe&0M.b%rI'7d)o4~VfZ*{#:}ETt$3J-zpc]lnh8,GwP_ND|jO 
  ```
* Metode enkripsi pada suatu direktori juga berlaku kedalam direktori lainnya yang ada didalamnya.

### Penyelesaian
Berikut merupakan kode fungsi untuk melakukan enkripsi dan dekripsi.
```
int key = 10;
char cipher[] = "9(ku@AW1[Lmvgax6q`5Y2Ry?+sF!^HKQiBXCUSe&0M.b%rI'7d)o4~VfZ*{#:}ETt$3J-zpc]lnh8,GwP_ND|jO";

void encrypt(char *x) 
{
	int xlength = strlen(x), xbegin = 0;
	int i;
	for(i = strlen(x); i >= 0; i--) {
		if(x[i] == '/') {
			break;
		}

		if(x[i] == '.') {
			xlength = i - 1;
		}
	}

	for (int i = 1; i < xlength; i++)
	{
		if(x[i] == '/') {
			xbegin = i;
		}
	}
	
	int ind;
	char *ptr;
	for(i = xbegin ; i < xlength; i++) {
		if(x[i] == '/') {
			continue;
		}

		ptr = strchr(cipher, x[i]);

		if(ptr) 
		{
			ind = ptr - cipher;
			x[i] = cipher[(ind + key) % strlen(cipher)];
		}
	}

	// printf("hasil encrypt %s\n", x);
}

void decrypt(char *y) 
{
	// printf("sebelum %s\n", y);

	int ylength = strlen(y), ybegin = 0;
	int i;
	
	for (int i = 1; i < ylength; i++)
	{
		if(y[i] == '/' || y[i+1] == '\0') {
			ybegin = i + 1;
			break;
		}
	}

	for(int i = strlen(y); i >= 0; i--) {
		// printf("y[i] -> %c\n", y[i]);
		if(y[i] == '/') {
			break;
		}
		if(y[i] == '.' && i == (strlen(y)-1)) {
			ylength = strlen(y);
			break;
		}
		if(y[i] == '.' && i != (strlen(y)-1)) {
			ylength = i - 1;
			break;
		}
	}

	int ind;
	char *ptr;
	for(i = ybegin ; i < ylength; i++) {
		if(y[i] == '/') {
			continue;
		}
		// printf("y masuk %c - ", y[i]);
		ptr = strchr(cipher, y[i]);

		if(ptr) 
		{
			ind = ptr - cipher - key;
			if (ind < 0)
			{
				ind = ind + strlen(cipher);
			}
			y[i] = cipher[ind];
		}
		// printf("y keluar %c\n", y[i]);
	}

	// printf("hasil decrypt %s\n", y);
}
```

Untuk melakukan enkripsi, kita perlu memanggil fungsi tersebut pada `xmp_readdir`
```
 while ((de = readdir(dp)) != NULL)
{
    struct stat st;
    memset(&st, 0, sizeof(st));
    st.st_ino = de->d_ino;
    st.st_mode = de->d_type << 12;

    if(strcmp(de->d_name, ".") == 0 || strcmp(de->d_name, "..") == 0) {
      continue;
    }

    char temp[1000];
    strcpy(temp, de->d_name);

    if(strncmp(path, "/encv1_", 7) == 0) {
      encrypt(temp);
      // printf("encrypted d_name %s\n", temp);
    }

    // printf("%s - > %s\n", de->d_name, temp);

    // res = (filler(buf, de->d_name, &st, 0));
    res = (filler(buf, temp, &st, 0));

    if (res != 0)
        break;
}
```
Setelah meng-enkripsi, kita perlu memanggil fungsi dekripsi pada setiap fungsi fuse
```
if(strncmp(path, "/encv1_", 7) == 0) {
  decrypt(temppath);
}
```

## SOAL 2
Pada soal 1 ini kita diminta melakukan enkripsi versi 2 :
* Jika sebuah direktori dibuat dengan awalan “encv2_”, maka direktori tersebut akan menjadi direktori terenkripsi menggunakan metode  
  enkripsi v2.
* Jika sebuah direktori di-rename dengan awalan “encv2_”, maka direktori tersebut akan menjadi direktori terenkripsi menggunakan metode 
  enkripsi v2.
* Apabila sebuah direktori terenkripsi di-rename menjadi tidak terenkripsi, maka isi direktori tersebut akan terdekrip.
* Setiap pembuatan direktori terenkripsi baru (mkdir ataupun rename) akan tercatat ke sebuah database/log berupa file.
* Pada enkripsi v2, file-file pada direktori asli akan menjadi bagian-bagian kecil sebesar 1024 bytes dan menjadi normal ketika diakses 
  melalui filesystem rancangan jasir. Sebagai contoh, file File_Contoh.txt berukuran 5 kB pada direktori asli akan menjadi 5 file kecil 
  yakni: File_Contoh.txt.000, File_Contoh.txt.001, File_Contoh.txt.002, File_Contoh.txt.003, dan File_Contoh.txt.004.
* Metode enkripsi pada suatu direktori juga berlaku kedalam direktori lain yang ada didalam direktori tersebut (rekursif).

### Penyelesaian
Berikut merupakan kode penyelesaian untuk soal nomer 2
```
void getPath(char *path, char *files, char temp[]){
    char string[1000] = "";
    strcpy(string, path);
    char * token = strtok(string, "/");
    while(strcmp(token, files)) {
        sprintf( temp, "%s/%s", temp, token);
        token = strtok(NULL, "/");
    }
}

char *getName(char* myStr) {
    char *retStr;
    char *lastExt;
    if (myStr == NULL) return NULL;
    if ((retStr = malloc (strlen (myStr) + 1)) == NULL) return NULL;
    strcpy (retStr, myStr);
    lastExt = strrchr (retStr, '.');
    if (lastExt != NULL)
        *lastExt = '\0';
    return retStr;
}

void combineFile(char *path){

    char temp[1000]="";
    char *files = rindex(path, '/');
    char *ext = rindex(files+1, '.');
    ext++;
    char *name = getName(files+1);
    char newFile[1000]="";
    getPath(path, files+1, temp);

    sprintf(newFile, "%s/%s", temp, name);

    int count=4;
    while(1){
        char partFile[1000]="";
        sprintf(partFile, "%s.%d", newFile, count);
        printf("%s\n",partFile);
        if(strstr(path, partFile) == NULL) break;
        count++;
        remove(partFile);
        sleep(2);
    }
        
}

long file_size(char *name){
    FILE *fp = fopen(name, "rb"); //must be binary read to get bytes

    long size=-1;
    if(fp)
    {
        fseek (fp, 0, SEEK_END);
        size = ftell(fp)+1;
        fclose(fp);
    }
    return size;
}

void splitFile(char *path){
    int segments=0, i, accum;
    FILE *fp1, *fp2;

    char filename[255];//base name for small files.
    sprintf(filename, "%s", path);
    char largeFileName[255];//change to your path
    sprintf(largeFileName, "%s", path);
    
    char smallFileName[255];
    char line[1080];
    long sizeFile = file_size(largeFileName);
    segments = sizeFile/SEGMENT + 1;//ensure end of file
    
    if(sizeFile<=1080) return;

    fp1 = fopen(largeFileName, "r");
    if(fp1)
    {
        for(i=0;i<segments;i++)
        {
            accum = 0;
            sprintf(smallFileName, "%s.%d", filename, i);
            fp2 = fopen(smallFileName, "w");
            if(fp2)
            {
                while(fgets(line, 1080, fp1) && accum <= SEGMENT)
                {
                    accum += strlen(line);//track size of growing file
                    fputs(line, fp2);
                }
                fclose(fp2);
            }
        }
        fclose(fp1);
    }
    remove(path);
}
```
Fungsi ini digunakan untuk memisahkan sebuah file menjadi beberapa file dengan ukuran lebih kecil setelah sebuah folder dirubah
menjadi `encv2_ ...`.

## SOAL 3
Tanpa mengurangi keumuman, misalkan suatu directory bernama dir akan tersinkronisasi dengan directory yang memiliki nama yang sama 
dengan awalan sync_ yaitu sync_dir. Persyaratan untuk sinkronisasi yaitu:
* Kedua directory memiliki parent directory yang sama.
* Kedua directory kosong atau memiliki isi yang sama. Dua directory dapat dikatakan memiliki isi yang sama jika memenuhi:
    * Nama dari setiap berkas di dalamnya sama.
    * Modified time dari setiap berkas di dalamnya tidak berselisih lebih dari 0.1 detik.
* Sinkronisasi dilakukan ke seluruh isi dari kedua directory tersebut, tidak hanya di satu child directory saja.
* Sinkronisasi mencakup pembuatan berkas/directory, penghapusan berkas/directory, dan pengubahan berkas/directory.

Jika persyaratan di atas terlanggar, maka kedua directory tersebut tidak akan tersinkronisasi lagi.
Implementasi dilarang menggunakan symbolic *links* dan *thread*.

### Penyelesaian


## SOAL 4
Pada soal ini kita diminta untuk membuat log system:

* Sebuah berkas nantinya akan terbentuk bernama "fs.log" di direktori *home* pengguna (/home/[user]/fs.log) yang berguna menyimpan 
  daftar perintah system call yang telah dijalankan.
* Agar nantinya pencatatan lebih rapi dan terstruktur, log akan dibagi menjadi beberapa level yaitu INFO dan WARNING.
* Untuk log level WARNING, merupakan pencatatan log untuk syscall rmdir dan unlink.
* Sisanya, akan dicatat dengan level INFO.
* Format untuk logging yaitu:

```
[LEVEL]::[yy][mm][dd]-[HH]:[MM]:[SS]::[CMD]::[DESC ...]
```

LEVEL    : Level logging
yy   	 : Tahun dua digit
mm    	 : Bulan dua digit
dd    	 : Hari dua digit
HH    	 : Jam dua digit
MM    	 : Menit dua digit
SS    	 : Detik dua digit
CMD     	 : System call yang terpanggil
DESC      : Deskripsi tambahan (bisa lebih dari satu, dipisahkan dengan ::)

Contoh format logging nantinya seperti:
```
INFO::200419-18:29:28::MKDIR::/iz1
INFO::200419-18:29:33::CREAT::/iz1/yena.jpg
INFO::200419-18:29:33::RENAME::/iz1/yena.jpg::/iz1/yena.jpeg
```

### Penyelesaian
Untuk membuat log system, kita perlu membuat fungsi
```
void logs(int level, char* cmd, char* desc1, char* temp)
{
    char data[1000];
    char lvl[100];
    char desc2[1000];

    FILE * fPtr;
    time_t t = time(NULL);
    struct tm tm = *localtime(&t);

    sprintf(lvl, level? "WARNING" : "INFO");
    if(strcmp(temp,"")) sprintf(desc2, "::%s", temp);
    
    fPtr = fopen("/home/kresna/fs.log", "a");
    sprintf(data, "%s::%d%d%d-%d:%d:%d::%s::%s%s\n", lvl, tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec, cmd, desc1, desc2);
    
    if(fPtr == NULL)
    {
        printf("Unable to create file.\n");
        exit(EXIT_FAILURE);
    }
    fputs(data, fPtr);
    fclose(fPtr);
}
```
pada setiap fungsi fuse, kita hanya perlu memanggil fungsi tersebut dan memberikan parameter levelnya.

Contoh :
```
logs(0, "RENAME", src, dst);
```
