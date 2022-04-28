

// the picture with the higher resolution defines the double loop

databig;
datasmall;

check bigwidth * 3;

for (int y = 0; y < biggerheight; y++)
{
    for (int x = 0; x < bigwidgth; x++)
    {
        BYTE bb, gb, rb, bs, gs, rs;

        bb = databig[realbytewidth * y + x * 3 + 0];
        gb = databig[realbytewidth * y + x * 3 + 1];
        rb = databig[realbytewidth * y + x * 3 + 2];

        bs = datasmall[xs * 3 + realwidthsmall * ys + 0];
        gs = datasmall[xs * 3 + realwidthsmall * ys + 1];
        rs = datasmall[xs * 3 + realwidthsmall * ys + 2];

        // there will be hazards and seg faults - use conditionals to catch these

        resb = bb * (1 - t) + bs * t;
    }
}