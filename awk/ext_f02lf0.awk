{
    for (j = 1; j <= 1; j++) {
	data[FNR,j] = $j
    }
} END {
    for (j = 1; j <= FNR; j++) {
      if (data[j, 1] > 0.0) {
	printf "%f\n", log(data[j, 1])
      }
    }
}
