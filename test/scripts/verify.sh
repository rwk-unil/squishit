#!/bin/bash

POSITIONAL=()
while [[ $# -gt 0 ]]
do
key="$1"

# Command line argument parsing from :
# https://stackoverflow.com/questions/192249/how-do-i-parse-command-line-arguments-in-bash
case $key in
    -f|--filename)
    FILENAME="$2"
    shift # past argument
    shift # past value
    ;;
    *)    # unknown option
    POSITIONAL+=("$1") # save it in an array for later
    shift # past argument
    ;;
esac
done
set -- "${POSITIONAL[@]}" # restore positional parameters

if [ -z "${FILENAME}" ]
then
    echo "Specify a filename with --filename, -f <filename>"
    exit 1
fi

echo "FILENAME        = ${FILENAME}"

TMPDIR=$(mktemp -d -t gtcompressor) || { echo "Failed to create temporary directory"; exit 1; }

echo "Temporary director : ${TMPDIR}"

function exit_fail_rm_tmp {
    rm -r ${TMPDIR}
    exit 1
}

../../console_app -c -f ${FILENAME} -o ${TMPDIR}/compressed.bin || { echo "Failed to compress ${FILANEM}"; exit_fail_rm_tmp; }
../../console_app -x -f ${TMPDIR}/compressed.bin -o ${TMPDIR}/uncompressed.bcf || { echo "Failed to uncompress ${FILANEM}"; exit_fail_rm_tmp; }

command -v bcftools || { echo "Failed to find bcftools, is it installed ?"; exit_fail_rm_tmp; }

echo
echo "Diff between original file and uncompressed compressed file :"
echo "We expect at least one line that differs, which is the bcftools_viewCommand="
echo

# Diffing two large VCF/BCFs will take huge amount of memory since diff is not
# a streaming program, it will load everything in memory first...
# E.g., 1KGP3 chr20 -> about 9 GB (uncompressed view output) times 2 (two files)
#diff <(bcftools view ${FILENAME}) <(bcftools view ${TMPDIR}/uncompressed.bcf) | tee ${TMPDIR}/difflog.txt
diff <(bcftools view ${FILENAME}) <(bcftools view ${TMPDIR}/uncompressed.bcf) > ${TMPDIR}/difflog.txt
DIFFLINES=$(wc -l ${TMPDIR}/difflog.txt | awk '{print $1}')
#echo $DIFFLINES
if [ ${DIFFLINES} -gt 4 ]
then
    echo
    echo "[KO] The files differ, check out ${TMPDIR}/difflog.txt"
    #exit_fail_rm_tmp # For dev
    exit 1
else
    echo
    echo "[OK] The files are the same"
fi

rm -r $TMPDIR
exit 0