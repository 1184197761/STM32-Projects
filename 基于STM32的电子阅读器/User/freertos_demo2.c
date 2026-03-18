/**
 ****************************************************************************************************
 * @file        main.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.4
 * @date        2022-01-04
 * @brief       FreeRTOS emWin 移植实验
 * @license     Copyright (c) 2020-2032, 广州市星翼电子科技有限公司
 ****************************************************************************************************
 * @attention
 *
 * 实验平台:正点原子 战舰F103开发板
 * 在线视频:www.yuanzige.com
 * 技术论坛:www.openedv.com
 * 公司网址:www.alientek.com
 * 购买地址:openedv.taobao.com
 *
 ****************************************************************************************************
 */

#include "freertos_demo.h"
#include "key.h"
#include <stdbool.h>
#include <math.h>
#include "tpad.h"
#include "beep.h"
#include "rtc.h"
#include "adc.h"
#include "lsens.h"
/******************************************************************************************************/
/*FreeRTOS配置*/
/* 宏定义 */
#define STACK_SIZE 128    /* 任务栈大小的最小推荐值 */
#define SCREEN_WIDTH 480  /*屏幕宽度*/
#define SCREEN_HEIGHT 320 /*屏幕高度*/
#define TITLE_FONT ((GUI_FONT *)GUI_FONT_32B_ASCII)
#define LIST_FONT (GUI_FontChinese_20)
#define FOOTER_FONT (GUI_FontChinese_16)
#define TITLE_HEIGHT (TITLE_FONT->YDist)
#define FOOTER_HEIGHT (FOOTER_FONT.YDist)
#define ITEM_HEIGHT (LIST_FONT.YDist)
#define LIST_HEIGHT (SCREEN_HEIGHT - TITLE_HEIGHT - 2 * FOOTER_HEIGHT)
#define LIST_ROWS (LIST_HEIGHT / ITEM_HEIGHT)          /* 书籍列表行数 */
#define TEXTBOX_HEIGHT (SCREEN_HEIGHT - FOOTER_HEIGHT) /* 文本框高度（屏幕高度-页脚高度） */

/* 全局变量 */
uint32_t slider_position;       /* 保存当前电容滑条触摸位置信息（不触摸时为高阻态，不保存） */
uint32_t slider_gesture;        /* 保存当前电容滑条触摸手势信息（按下、单击、双击和双击拖动） */
uint32_t button_symbol;         /* 保存按键符号（ABCD分别为机械键盘1、2和电容键盘1、2） */
uint32_t capBTN0_untouched = 1, /* 外部变量，电容按键0、1当前是否未触摸（抬起） */
    capBTN1_untouched = 1;
extern GUI_FONT GUI_FontChinese_12, /* 外部变量，中文字体（新宋体），已修改Y轴距离和基线位置，程序中可根据需要自行修改 */
    GUI_FontChinese_16,
    GUI_FontChinese_20;

Book books[] = {
    /* 书籍数组 */
    {"You Have Only One Life",
     "\tThere are moments in life when you miss someone so much that you just want to pick them from your dreams and hug them for real! Dream what you want to dream;go where you want to go;be what you want to be,because you have only one life and one chance to do all the things you want to do. \n\
     \tMay you have enough happiness to make you sweet,enough trials to make you strong,enough sorrow to keep you human, enough hope to make you happy ? Always put yourself in others'shoes.If you feel that it hurts you, it probably hurts the other person, too.\n\
     The happiest of people don 't necessarily have the best of everything;they just make the most of everything that comes along their way.Happiness lies for those who cry,those who hurt, those who have searched,and those who have tried,for only they can appreciate the importance of people who have touched their lives.Love begins with a smile, grows with a kiss and ends with a tear.The brightest future will always be based on a forgotten past, you can' t go on well in lifeuntil you let go of your past failures and heartaches.\n\
     \tWhen you were born, you were crying and everyone around you was smiling.Live your life so that when you die, you're the one who is smiling and everyone around you is crying.\n\
     \tPlease send this message to those people who mean something to you, to those who have touched your life in one way or another, to those who make you smile when you really need it, to those that make you see the brighter side of things when you are really down, to those who you want to let them know that you appreciate their friendship.And if you don 't, don' t worry, nothing bad will happen to you, you will just miss out on the opportunity to brighten someone's day with this message.",
     1, NULL},
    {"你只有一次生命",
     "\t生命中有些时刻，你会如此想念某人，以至于你只想从梦中把他们拣出来，真正拥抱他们！做你想做的梦；去你想去的地方；成为你想成为的人，因为你只有一次生命和一次机会去做所有你想做的事。\n\
     \t你有足够的快乐让你甜蜜，足够的考验让你坚强，足够的悲伤让你有人性，足够的希望让你快乐吗？总是设身处地为别人着想。如果你觉得它伤害了你，它可能也伤害了其他人。\n\
     \t最幸福的人不一定拥有最好的一切；他们只会充分利用沿途的一切。幸福属于那些哭泣的人，那些受伤的人，那些寻找过的人，那些尝试过的人，因为只有他们才能体会到那些影响过他们生活的人的重要性。爱从微笑开始，在亲吻中成长，在眼泪中结束。最光明的未来总是建立在忘记过去的基础上，只有放下过去的失败和心痛，你的生活才能过得很好。\n\
     \t当你出生时，你在哭泣，你周围的人都在微笑。过好你的生活，这样当你死的时候，你就是那个微笑的人，而你周围的人都在哭泣。\n\
     \t请将此信息传达给那些对你有意义的人，那些以某种方式影响你生活的人，那些在你真正需要时让你微笑的人，那些在你情绪低落时让你看到事情光明面的人，那些你想让他们知道你欣赏他们友谊的人。如果你不这样做，别担心，不会有什么不好的事情发生在你身上，你只会错失用这条信息来照亮别人一天的机会。",
     1, NULL},
    {"book 3", "This is the third book!", 1, NULL},
    {"book 4", "This is the fourth book!", 1, NULL},
    {"book 5", "This is the firth book!", 1, NULL},
    {"book 6", "This is the sixth book!", 1, NULL},
    {"book 7", "This is the seventh book!", 1, NULL},
    {"book 8", "This is the eighth book!", 1, NULL},
};

/******************************************************************************************************/

/**
 * @brief       FreeRTOS例程入口函数
 * @param       无
 * @retval      无
 */
void freertos_demo(void)
{
    xTaskCreate((TaskFunction_t)StartupTask,           /* 任务函数 */
                (const char *)"StartupTask",           /* 任务名称 */
                (uint16_t)STACK_SIZE,                  /* 任务堆栈大小 */
                (void *)NULL,                          /* 传入给任务函数的参数 */
                (UBaseType_t)configMAX_PRIORITIES - 2, /* 任务优先级 */
                (TaskHandle_t *)NULL);                 /* 任务句柄 */
    vTaskStartScheduler();
}

/**
 * @brief       StartupTask
 * @param       pvParameters : 传入参数(未用到)
 * @retval      无
 */
void StartupTask(void *pvParameters)
{
    __HAL_RCC_CRC_CLK_ENABLE();
    GUI_Init();             /* 初始化图形界面 */
    GUI_UC_SetEncodeUTF8(); /* 设置UTF-8编码用于显示汉字 */
    GUI_Clear();            /* 清除图形界面 */
    key_init();
    tpad_init(2);
    beep_init();
    rtc_init();
    adc_temperature_init();
    lsens_init();
    // taskENTER_CRITICAL();   /* 进入临界区 */
    /* 创建任务1 */
    xTaskCreate(KeyTask, "KeyTask", STACK_SIZE, NULL, tskIDLE_PRIORITY + 2, NULL);
    /* 创建任务2 */
    xTaskCreate(LcdTask, "LcdTask", STACK_SIZE * 10, NULL, tskIDLE_PRIORITY + 1, NULL);
    vTaskDelete(NULL); /* 删除开始任务 */
    // taskEXIT_CRITICAL(); /* 退出临界区 */
}

/**
 * @brief       KeyTask
 * @param       pvParameters : 传入参数(未用到)
 * @retval      无
 */
void KeyTask(void *pvParameters)
{
    int key = 0, tpad = 0;
    while (true)
    {
        key = key_scan(1);
        tpad = tpad_scan(0);
        if (key || tpad)
        {
            BEEP(true);
        }
        vTaskDelay(20);
        BEEP(false);
    }
}

/**
 * @brief       LcdTask
 * @param       pvParameters : 传入参数(未用到)
 * @retval      无
 */
void LcdTask(void *pvParameters)
{
    char title[32];        /* 标题缓冲区 */
    uint8_t listpage = 0;  /* 书籍列表页 */
    uint8_t index_cur = 0; /* 当前索引（对应书籍数组下标值，这里不能使用index，因其在arm_math.h中已定义） */
    uint8_t books_num =    /* 书籍数量 */
        sizeof(books) / sizeof(*books);

    while (true)
    {
        /* 这里不需要清除屏幕是因为未翻页文本可以完美覆盖 */
        GUI_SetFont(TITLE_FONT);                                  /* 设置软件名字体 */
        GUI_DispStringHCenterAt("E-Reader", SCREEN_WIDTH / 2, 0); /* 居中打印软件名 */
        GUI_GotoXY(0, TITLE_FONT->YDist);                         /* 跳转到文本框首行，这里换行会与标题左对齐 */
        GUI_SetFont(&LIST_FONT);                                  /* 设置主页字体 */

        if (index_cur / LIST_ROWS != listpage) /* 若书籍不在当前列表页则更新列表 */
        {
            GUI_ClearRect(0, TITLE_HEIGHT, 480, SCREEN_HEIGHT - FOOTER_HEIGHT); /* 清除列表框，防止翻页时字体残留 */
            listpage = index_cur / LIST_ROWS;                                   /* 切换到当前书籍所在列表页 */
        }
        /* 遍历当前列表页，首行索引=列表页*行数 */
        for (int i = listpage * LIST_ROWS; i < books_num && i < (listpage + 1) * LIST_ROWS; i++)
        {
            if (i == index_cur) /* 若书籍选中（当前行为选中行） */
            {
                // GUI_SetTextMode(GUI_TEXTMODE_REV); /* 当前行反色显示（修改字体YDist后无法显示，原因不明） */
                GUI_SetTextStyle(GUI_TS_UNDERLINE); /* 当前行添加下划线 */
            }
            sprintf(title, "%03d %s\n", i + 1, books[i].name); /* 格式化赋值标题 */
            GUI_DispString(title);                             /* 显示标题 */
            GUI_SetTextStyle(GUI_TS_NORMAL);                   /* 恢复正常显示 */
        }
        GUI_SetFont(&FOOTER_FONT); /* 设置主页字体 */
        rtc_get_time();

        char data[100];
        GUI_DispStringAt("</> 按键0/1-切换书籍 | 长按-快速切换 ", 0, SCREEN_HEIGHT - 2 * FOOTER_HEIGHT);
        sprintf(data, "%2d.%02d.%02d-%02d:%02d:%02d", calendar.year, calendar.month, calendar.date, calendar.hour, calendar.min, calendar.sec);
        GUI_DispString(data);
        GUI_DispString("\n(X) 按键2-进入阅读 | 触摸按键-切换主题 ");
        sprintf(data, "%.2f摄氏度 ", adc_get_temperature() / 100.0);
        GUI_DispString(data);
        sprintf(data, "%d光照度", lsens_get_val());
        GUI_DispString(data);

        int key = 0, tpad = 0;

        while (!key && !tpad) /* 等待直到检测到输入 */
        {
            key = key_scan(1);
            tpad = tpad_scan(0);
        }
        BEEP(true);
        vTaskDelay(100);
        BEEP(false);

        switch (key) /* 检测滑条左/右划动手势，修改了长度和时间阈值，使之更容易触发 */
        {
        // /* 左划 */
        // case KEY0_PRES:
        //     if (listpage < (books_num - 1) / LIST_ROWS) /* 当前不是最后一页 */
        //     {
        //         index_cur = listpage * LIST_ROWS + LIST_ROWS; /* 选择下页第一行 */
        //     }
        //     break;
        // /* 右划 */
        // case KEY1_PRES:
        //     if (listpage > 0) /* 当前不是第一页 */
        //     {
        //         index_cur = listpage * LIST_ROWS - LIST_ROWS; /* 选择上页第一行 */
        //     }
        //     break;
        /* 点击，这里不用LIFT_OFF是因为松开后无法判断触摸位置（为高阻态）
           不保存高阻态值后，可以使用LIFT_OFF，但由于开启了CLICK手势，必须使用SINGLE_CLICK和DOLBLE_CLICK(快速单击会触发双击，不加入可能导致第二次点击无反应）
           这里开启手势是因为阅读界面要用到双击拖拽
        */
        case KEY0_PRES:
            if (index_cur > 0) /* 点击左半区域，如果不是第一行则执行语句 */
            {
                index_cur--; /* 选择上一行 */
            }
            break;
        case KEY1_PRES:
            if (index_cur < books_num - 1) /* 右半区域，这里仍然要判断触摸位置 */
            {
                index_cur++; /* 选择下一行 */
            }
            break;
        /* 若获取到按键0消息 */
        case WKUP_PRES:
            ReadView(&books[index_cur]); /* 进入阅读视图 */
            GUI_Clear();                 /* 返回时清除屏幕防止文本残留 */
            break;
        }
        if (tpad)
        {
            GUI_SetBkColor(~GUI_GetBkColor()); /* 反转背景色 */
            GUI_SetColor(~GUI_GetColor());     /* 反转前景色 */
            GUI_Clear();                       /* 以新的背景色覆盖屏幕 */
        }
    }
}

/* 阅读视图
   根据当前书籍显示阅读区和和状态栏
 */
void ReadView(Book *book)
{
    char footer[64]; /* 页脚缓冲区 */
    float progress;  /* 阅读进度 */
    int pageNum;     /* 书籍页数 */
    int rowNum;
    int page_cur = book->marker; /* 当前页码 */
    GUI_FONT *font_cur =         /* 阅读字体 */
        book->font ? book->font : &GUI_FontChinese_16;
    GUI_RECT rect = /* 文本框矩形 */
        {0, (1 - page_cur) * TEXTBOX_HEIGHT, SCREEN_WIDTH, TEXTBOX_HEIGHT - 1};

    GUI_SetFont(font_cur); /* 装载阅读字体，提前设置是因为字体大小会影响GUI_WrapGetNumLines()返回值 */
    /* 每页行数 = 文本框高度/每行高度（总行数/行数很可能有余数，应向上取整） */
    rowNum = TEXTBOX_HEIGHT / font_cur->YDist;
    /* 书籍页数 = 文本总行数/每页行数 */
    pageNum = ceil(GUI_WrapGetNumLines(book->content, rect.x1, GUI_WRAPMODE_CHAR) / rowNum);
    while (true)
    {
        GUI_SetFont(font_cur);                             /* 装载阅读字体 */
        GUI_ClearRect(rect.x0, rect.y0, rect.x1, rect.y1); /* 清除上一页面的字体残留，缺点是翻页闪屏现象比较严重 */
        GUI_DispStringInRectWrap(                          /* 在文本框中打印文本信息，左对齐并以字符方式换行（单词会被拆开，但是比较整齐） */
                                 book->content, &rect, GUI_TA_LEFT, GUI_WRAPMODE_CHAR);
        GUI_SetFont(&FOOTER_FONT); /* 单独设置页脚字体，避免缩放导致页脚溢出 */
        sprintf(footer, "</> 页码 %03d/%03d  (X) 字号 %d | 退出", page_cur, pageNum, font_cur->YSize);
        GUI_DispStringAt(footer, 0, rect.y1 + 1); /* 在文本框下方打印页脚信息 */

        do /* 循环检测滑条手势和按键消息 */
        {
            int key = 0, tpad = 0;

            while (!key && !tpad) /* 等待直到检测到输入 */
            {
                key = key_scan(1);
                tpad = tpad_scan(0);
            }

            BEEP(true);
            vTaskDelay(100);
            BEEP(false);

            book->marker = page_cur; /* 保存当前页面以便循环判断是否翻页 */
            book->font = font_cur;   /* 保存当前字体以便循环判断是否缩放 */
            switch (key)             /* 检测滑条手势 */
            {
            /* 左右区域按下，TOUCHDOWN可加快翻页响应 */
            case KEY0_PRES:
                if (page_cur > 1) /* 左边缘 */
                {
                    page_cur--;
                    rect.y0 += TEXTBOX_HEIGHT;
                }
                break;
            case KEY1_PRES:
                if (page_cur < pageNum) /* 右边缘 */
                {
                    page_cur++;
                    rect.y0 -= TEXTBOX_HEIGHT;
                }
                break;
            /* 中间区域双击并拖动，拖动中如果断触应停止修改，不保存高阻态后可以忽略 */
            // case CapSense_TMG_CLICK_AND_DRAG:
            //     // if (slider_position != CapSense_SLIDER_NO_TOUCH) /* 防止断触时0XFFFF造成页码和文本框位置异常 */
            //     // {
            //     page_cur = slider_position * pageNum / (double)101 + 1; /* 按触摸位置等比例地更新页码 */
            //     rect.y0 = (1 - page_cur) * TEXTBOX_HEIGHT;              /* 根据当前页码更新文本框位置 */
            //     // }
            //     break;

            /* 按键1 */
            case WKUP_PRES:
                book->marker = page_cur; /* 保存书籍书签 */
                book->font = font_cur;   /* 保存书籍字体 */
                return;                  /* 返回书籍列表 */
            }
            if (tpad)
            {
                /* 按键0 */
                switch (font_cur->YSize) /* 检测当前字号 */
                {
                case 16:
                    font_cur = &GUI_FontChinese_20;
                    break;
                case 20:
                    font_cur = &GUI_FontChinese_12;
                    break;
                case 12:
                    font_cur = &GUI_FontChinese_16;
                    break;
                }
                progress = (double)page_cur / pageNum; /* 保存当前阅读进度 */
                GUI_SetFont(font_cur);                 /* 由于GUI_WrapGetNumLines()返回值与字号有关，提前设置可防止页数错误 */
                pageNum = ceil(GUI_WrapGetNumLines(book->content, rect.x1, GUI_WRAPMODE_CHAR) / (double)(TEXTBOX_HEIGHT / font_cur->YDist));
                page_cur = ceil(progress * pageNum);       /* 按阅读进度更新当前页码 */
                rect.y0 = (1 - page_cur) * TEXTBOX_HEIGHT; /* 根据当前页码更新文本框位置 */
            }
        } while (page_cur == book->marker && font_cur->YSize == book->font->YSize); /* 若页码和字号未改变则不刷新屏幕，避免高频刷新 */
    }
}