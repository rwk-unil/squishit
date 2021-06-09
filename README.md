# VCF / BCF Compressor

VCF / BCF Genotype data compressor based on positional Burrows-Wheeler transform (PBWT) and 16-bit Word Aligned Hybrid (WAH) encoding.

Variants are left in BCF format, genotype data is custom encoded. The genotype data can then be further compressed with standard tools such as gzip.

## Build

This build requires GCC 8+ because modern C++17 features are used.

```shell
# Clone
git clone https://github.com/rwk-unil/pbwt_exp.git #TODO Public repo
cd pbwt_exp
# Clone and build htslib (if you already have htslib set Makefile accordingly and skip)
git submodule update --init htslib
cd htslib
autoheader
autoconf
./configure
make
sudo make install
sudo ldconfig
cd ..
# Build application
make
```

## Run

### Compression

```shell
# ./console_app <-c|-x> -f <input file> -o <output file>
mkdir output

# Compression :
./console_app -c -f /path/to/my/data/chr20.bcf -o output/chr20.bin
# This will output two files in output
# output/chr20.bin which is the samples and genotype data in binary encoded format (can still be compressed e.g., with gzip)
# output/chr20.bin_var.bcf which is the variant data, can be opened with bcftools
```

Options :
- `--iota` allows to use natural ordering for checkpoints instead of saving the permutation arrays in the binary file, this results in smaller binary file for collection with many samples. This has no noticeable impact on speed.

### Extraction

```shell
# Extraction (requires both files generated above) :
./console_app -x -f output/chr20.bin -o output/chr20.bcf # To compressed BCF
./console_app -x -f output/chr20.bin > output/chr20.bcf # Alternative command (uncompressed BCF)
```

#### Region extraction
```shell
# Extraction (requires both files generated above) :
./console_app -x -r "20:200000-200100" -f output/chr20.bin -o output/chr20.bcf # To compressed BCF
./console_app -x -r "20:200000-200100" -f output/chr20.bin | bcftools view # Pipes uncompressed BCF
# The above command is much faster than decompressing and using -r in bcftools
# because only the chosen regions are decompressed, both generate the same result
```

#### Sample extraction
```shell
# Extraction (requires both files generated above) :
./console_app -x -s HG00101,NA12878 -f output/chr20.bin -o output/chr20.bcf # To compressed BCF
./console_app -x -s HG00101,NA12878 -f output/chr20.bin | bcftools view # Pipes uncompressed BCF
```

### Pipe into bcftools

```shell
# Or pipe directly into bcftools (some examples) :
./console_app -x -f output/chr20.bin | bcftools view | less
./console_app -x -f output/chr20.bin | bcftools view -s HG00111,NA12878 | less
./console_app -x -f output/chr20.bin | bcftools stats > output/chr20_stats.txt
```

## Notes / TODO

- Rename the compressor (currently named console_app ...)
- Only outputs data as phased for the moment
- Handle missing
- ~~Only supports bi-allelic sites for the moment~~ Done !
    - (not needed anymore) Convert multi-allelic VCF/BCF to bi-allelic with bcftools :  
      ```shell
      bcftools norm -m any multi_allelic.bcf -o bi_allelic.bcf -O b
      ```

## Further works

- Extraction
- Filtering
- Based on the block compression scheme for faster access
- ~~Based on permutation sub sampling for faster / parallel access~~ Done !