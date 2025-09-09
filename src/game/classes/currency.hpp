#pragma once

struct Currency {
    char pad_0000[0xB8BC];
    int Units;
    int Nanites;
    int Quicksilver;
};
