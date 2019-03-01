# awk -f hist_lf02f0.awk lf0.hist > f0.hist
 
{
    for (j = 1; j <= 2; j++) {
	data[FNR,j] = $j
    }
} END {
    for (j = 4; j <= FNR; j++) {
	printf "%f %f\n", exp(data[j,1]), data[j, 2]
    }
}
