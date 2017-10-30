#ifndef GLOBAL_H
#define GLOBAL_H

enum Color
{
    BLUE,
    YELLOW,
    WHITE,
    UNKNOWN
};

static const char *kChars[] =
    {
        "0", "1", "2",
        "3", "4", "5",
        "6", "7", "8",
        "9",
        /*  10  */
        "A", "B", "C",
        "D", "E", "F",
        "G", "H",
        "J", "K", "L",
        "M", "N",
        "P", "Q", "R",
        "S", "T", "U",
        "V", "W", "X",
        "Y", "Z",
        /*  24  */
        "zh_cuan", "zh_e", "zh_gan",
        "zh_gan1", "zh_gui", "zh_gui1",
        "zh_hei", "zh_hu", "zh_ji",
        "zh_jin", "zh_jing", "zh_jl",
        "zh_liao", "zh_lu", "zh_meng",
        "zh_min", "zh_ning", "zh_qing",
        "zh_qiong", "zh_shan", "zh_su",
        "zh_sx", "zh_wan", "zh_xiang",
        "zh_xin", "zh_yu", "zh_yu1",
        "zh_yue", "zh_yun", "zh_zang",
        "zh_zhe"};

const int kCharsNumber = 34;
const int kChineseNumber = 31;
const int kCharsTotalNumber = 65;

const int kCharSize = 20;
const int kPlateMaxCharNum = 7;

const int kANNDataSize = 10;
const int kANNZhDataSize = 20;
const int kANNZhGrayWidth = 32;
const int kANNZhGrayHeight = 20;

const int kANNMinSampleNum = 350;
const int kANNZhMinSampleNum = 100;
const int kANNZhGrayMinSampleNum = 1000;

static const char *kANNModelPath = "./model/ann.xml";
static const char *kANNZhModelPath = "./model/ann_chinese.xml";
static const char *kANNZhGrayModelPath = "./model/ann_chinese_gray.xml";

static const char *kANNSamplePath = "./res/sample/all_binary_chars";
static const char *kANNZhSamplePath = "./res/sample/onlyzh_binary_chars";
static const char *kANNZhGraySamplePath = "./res/sample/onlyzh_gray_chars";

const bool kANNSaveSynthetic = false;
static const char *kANNSyntheticPath = "./res/sample/synthetic/all_binary_chars";
static const char *kANNZhSyntheticPath = "./res/sample/synthetic/onlyzh_binary_chars";
static const char *kANNZhGraySyntheticPath = "./res/sample/synthetic/onlyzh_gray_chars";

static const char *kLocaterDisplayPath = "./res/locater";
static const char *kDeskewDisplayPath = "./res/deskew";

static const char *kSVMModelPath = "./model/svm.xml";
static const char *kSVMIsPlateTrainSamplePath = "./res/sample/is_plate/train";
static const char *kSVMNoPlateTrainSamplePath = "./res/sample/no_plate/train";
static const char *kSVMIsPlateTestSamplePath = "./res/sample/is_plate/test";
static const char *kSVMNoPlateTestSamplePath = "./res/sample/no_plate/test";

const int kSVMPlateWidth = 136;
const int kSVMPlateHeight = 36;

#endif