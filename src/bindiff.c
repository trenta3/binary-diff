#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <errno.h>
#include <ctype.h>
#include <stdarg.h>
#include <stdlib.h>
#include "debug.h"

// Dichiariamo il fattore di qualità (quanti byte devono essere ripetuti affinchè
// si guadagni davvero spazio a codificare tutto nel modo furbo)
// È messo a 8 visto che se scriviamo i dati raw abbiamo bisogno di 8 byte più la
// lunghezza effettiva dei dati, mentre riferendosi a dati presenti in A abbiamo
// bisogno di 16 byte fissi. Quindi conviene quando 16 =< 8 + S, ovvero S >= 8
#define MIN_VALID_BYTES 8UL

#define false 0
#define true 1

// Dichiariamo alcuni tipi che ci saranno comodi
typedef unsigned long int ulint;

// Uso: bindiff fileA fileB difffile
int main (int argc, char *argv[]) {
	FILE *difffile;
	int fdA = 0, fdB = 0, status = 0;
	size_t Asize, Bsize;
	struct stat Astat, Bstat;
	const char *Amap, *Bmap;

	// Stampiamo il numero di versione
	printf("***bindiff v. 001***\n");

	// Vediamo se ci hanno passato il numero giusto di argomenti
	checkexit (argc < 4, "Vanno passati almeno tre argomenti.\nUso: bindiff fileA fileB difffile\nProduce un file delle differenze tra A e B.\n");

	// Altrimenti apriamo i tre file controllando che non ci siano errori
	fdA = open(argv[1], O_RDONLY);
	checkexit (fdA < 0, "open %s RDONLY failed", argv[1]);

	status = fstat(fdA, &Astat);
	CHECK (status < 0, "stat %s failed", argv[1]);
	ONERROR close(fdA); exit(EXIT_FAILURE); ENDCHECK

	Asize = Astat.st_size;
	Amap = mmap(0, Asize, PROT_READ, MAP_SHARED, fdA, 0);
	CHECK (Amap == MAP_FAILED, "mmap %s failed", argv[1]);
	ONERROR close(fdA); exit(EXIT_FAILURE); ENDCHECK

	fdB = open(argv[2], O_RDONLY);
	CHECK (fdB < 0, "open %s RDONLY failed", argv[2]);
	ONERROR close(fdA); exit(EXIT_FAILURE); ENDCHECK

	status = fstat(fdB, &Bstat);
	CHECK (status < 0, "stat %s failed", argv[2]);
	ONERROR close(fdA); close(fdB); exit(EXIT_FAILURE); ENDCHECK

	Bsize = Bstat.st_size;
	Bmap = mmap(0, Bsize, PROT_READ, MAP_SHARED, fdB, 0);
	CHECK (Bmap == MAP_FAILED, "mmap %s failed", argv[2]);
	ONERROR close(fdA); close(fdB); exit(EXIT_FAILURE); ENDCHECK

	difffile = fopen(argv[3], "wb");
	CHECK (difffile == NULL, "Open %s RDWR failed", argv[3]);
	ONERROR close(fdA); close(fdB); exit(EXIT_FAILURE); ENDCHECK

	// --- FINE DEI CONTROLLI --- INIZIA IL CODICE VERO ---
	log_info("File aperti. Inizio della ricerca differenze.");
	// Scriviamo prima l'header nel file delle differenze
	char header[] = "BDIFv001";
	CHECK (fwrite(header, strlen(header), 1, difffile) != 1, "fwrite failed to difffile");
	ONERROR fclose(difffile); close(fdA); close(fdB); exit(EXIT_FAILURE); ENDCHECK
	log_debug("Scritto header in difffile");

	// firstBraw tiene traccia del primo carattere di B che non abbiamo sistemato in una stringa di A
	ulint posB, firstBraw;
	for (firstBraw = 0, posB = 0; posB < Bsize; ) {
		// Cerchiamo la più grande stringa in A che coincide con quella che inizia in questo punto
		ulint curpos, ptpos, curmatchlen, ptmatchlen;
		curpos = ptpos = curmatchlen = ptmatchlen = 0;
		for ( ; curpos < Asize; curpos++) {
			// Calcoliamo il numero di posizioni di matching tra B[posB, ...] e A[posA, ...]
			ulint awrk = curpos, bwrk = posB;
			curmatchlen = 0;
			while (awrk < Asize && bwrk < Bsize && Amap[awrk] == Bmap[bwrk]) { awrk++; bwrk++; curmatchlen++; }
			// Ora se è maggiore dell'ultima calcolata ce lo sostituiamo
			if (curmatchlen > ptmatchlen) {
				ptmatchlen = curmatchlen;
				ptpos = curpos;
			}
		}
		// TODO: ci preoccuperemo in realtà di controllare e dividere in casi per firstBraw != posB
		// A questo punto in ptpos e ptmatchlen abbiamo la più grossa stringa in A che coincide
		// con gli elementi di B in questo punto. Se è maggiore o uguale di MIN_VALID_BYTES
		// allora la scriviamo in difffile, premurandoci di salvare i byte che sono raw
		log_debug("End of searching best string match for byte number %ld", posB);
		if (ptmatchlen >= MIN_VALID_BYTES) {
			if (firstBraw != posB) {
				// Ci sono dei raw byte da scrivere su file assolutamente
				// Si scrive prima datalength con MSB settato a 0 e poi viene scritto il pezzo di dati grezzi di B
				ulint datalength = (posB - firstBraw) & (~0UL >> 1);
				CHECK (fwrite(&datalength, sizeof(ulint), 1, difffile) != 1, "fwrite failed in writing raw data with datalength = %d", datalength);
				ONERROR fclose(difffile); close(fdA); close(fdB); exit(EXIT_FAILURE); ENDCHECK
				CHECK (fwrite(&Bmap[firstBraw], datalength, 1, difffile) != 1, "fwrite failed in writing raw data from firstBraw = %d", firstBraw);
				ONERROR fclose(difffile); close(fdA); close(fdB); exit(EXIT_FAILURE); ENDCHECK
				log_debug("Writing raw data byte to difffile");
			}
			// Scriviamo gli estremi della posizione dei byte della stringa di A
			ulint lengthofpiece = ptmatchlen | ~(~0UL >> 1);
			CHECK (fwrite(&lengthofpiece, sizeof(ulint), 1, difffile) != 1, "fwrite failed in writing A-data with lengthofpiece = %d", lengthofpiece);
			ONERROR fclose(difffile); close(fdA); close(fdB); exit(EXIT_FAILURE); ENDCHECK
			CHECK (fwrite(&ptpos, sizeof(ulint), 1, difffile) != 1, "fwrite failed in writing A-data with ptpos = %d", ptpos);
			ONERROR fclose(difffile); close(fdA); close(fdB); exit(EXIT_FAILURE); ENDCHECK
			log_debug("Writing compressed bytes to difffile");
			// And then we set the new Bposition after the end of the found string
			posB += ptmatchlen;
			firstBraw = posB;
		} else {
			// Non abbiamo convenienza a scrivere la stringa trovata, quindi facciamo soltanto avanzare posB
			log_debug("Best string match of %ld is not enough", ptmatchlen);
			posB++;
		}
	}
	if (firstBraw != posB) {
		// Abbiamo dei dati rimasti da scrivere su file
		ulint datalength = (posB - firstBraw) & (~0UL >> 1);
		CHECK (fwrite(&datalength, sizeof(ulint), 1, difffile) != 1, "fwrite failed in writing raw data with datalength = %d", datalength);
		ONERROR fclose(difffile); close(fdA); close(fdB); exit(EXIT_FAILURE); ENDCHECK
		CHECK (fwrite(&Bmap[firstBraw], datalength, 1, difffile) != 1, "fwrite failed in writing raw data from firstBraw = %d", firstBraw);
		ONERROR fclose(difffile); close(fdA); close(fdB); exit(EXIT_FAILURE); ENDCHECK
		log_debug("Writing last bytes to file");
	}
	// Dovrebbe essere terminato
	fclose(difffile); close(fdA); close(fdB);
	log_info("File chiusi. Programma terminato");
	return 0;
}