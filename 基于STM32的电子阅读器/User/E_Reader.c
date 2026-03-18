// 包含主程序头文件 E_Reader.h
#include "E_Reader.h"

// 包含标准 C 库头文件
#include <stdbool.h>
#include <math.h>

// 包含其他自定义头文件
#include "key.h"
#include "tpad.h"
#include "beep.h"
#include "rtc.h"
#include "adc.h"
#include "lsens.h"

// 定义宏常量
#define STARTUP_TASK_NAME "StartupTask"                  // 启动任务名字
#define STACK_SIZE 128                                   // 栈大小
#define STARTUP_TASK_STACK_SIZE STACK_SIZE               // 启动任务的栈大小
#define STARTUP_TASK_PRIORITY (configMAX_PRIORITIES - 2) // 启动任务的优先级
#define SCREEN_WIDTH 480                                 // 屏幕宽度
#define SCREEN_HEIGHT 320                                // 屏幕高度

#define FONT_TITLE GUI_Font32B_ASCII   // 标题字体
#define FONT_LIST GUI_FontChinese_20   // 列表字体
#define FONT_FOOTER GUI_FontChinese_16 // 底部字体

#define LIST_HEIGHT (SCREEN_HEIGHT - FONT_TITLE.YDist - 2 * FONT_FOOTER.YDist) // 列表高度
#define LIST_ROWS (LIST_HEIGHT / FONT_LIST.YDist)                              // 列表行数

#define TEXTBOX_HEIGHT (SCREEN_HEIGHT - FONT_FOOTER.YDist) // 文本框高度

bool tick = false; // 布尔类型变量 tick，初始值为 false

// 引用外部声明的字体变量
extern GUI_FONT GUI_FontChinese_12,
    GUI_FontChinese_16,
    GUI_FontChinese_20;

/******************************************************************************************************
 *                                      E-Reader Startup Task                                          *
 ******************************************************************************************************/

/**
 * @brief StartupTask任务,用于初始化任务和硬件资源
 * @param pvParameters 传入参数(未用到)
 */
void StartupTask(void *pvParameters)
{
    __HAL_RCC_CRC_CLK_ENABLE(); // 使能CRC时钟
    GUI_Init();                 // 图形界面初始化
    GUI_UC_SetEncodeUTF8();     // 设置编码UTF-8
    GUI_Clear();                // 清空屏幕显示
    key_init();                 // 机械按键初始化
    tpad_init(2);               // 触摸按键初始化
    beep_init();                // 蜂鸣器初始化
    rtc_init();                 // 实时时钟初始化
    adc_temperature_init();     // 温度传感器初始化
    lsens_init();               // 光线传感器初始化

    // 创建一个1s的定时器，并启动该定时器，绑定了一个回调函数 TimerCallback
    xTimerStart(xTimerCreate("Timer", pdMS_TO_TICKS(1000), pdTRUE, NULL, TimerCallback), portMAX_DELAY);

    // 创建一个LCD任务，使用默认优先级，根据应用实际情况可以修改优先级
    xTaskCreate(LcdTask, "LcdTask", STACK_SIZE * 10, NULL, tskIDLE_PRIORITY + 1, NULL);

    // 删除任务本身
    vTaskDelete(NULL);
}

/******************************************************************************************************
 *                                      Others Functions                                               *
 ******************************************************************************************************/

Book books[] = {
    // 声明8本书的信息
    {"You Have Only One Life",
     // 书籍内容（包含转义符\表示跨行）
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
    {"The Magic of Reality",
     "From the evolution of life, to the workings of the universe, this book explains it all. Let Richard Dawkins guide you on a journey from atoms to animals, and from molecules to myths, as he explains the mysteries of science in an approachable, engaging way.",
     1, NULL},
    {"Bad Blood",
     "The fascinating true story of a Silicon Valley startup that promised to revolutionize the blood testing industry, but became mired in deceit and corruption, leading to a shocking downfall.",
     1, NULL},
    {"The Art of War",
     "This ancient Chinese military treatise is still influential today as a strategy guide for business people and politicians. Sun Tzu's teachings on strategy, tactics, and winning battles apply just as much to the modern world as they did centuries ago.",
     1, NULL},
    {"The Da Vinci Code",
     "Robert Langdon, a visiting Harvard professor, is drawn into a thrilling adventure centered around the mystery of the Holy Grail. As he decodes clues and solves puzzles, he unravels a conspiracy that could shake the foundations of Christianity.",
     1, NULL},
    {"Dune",
     "In a distant future, the desert planet Arrakis is the only source of the prized spice melange, which grants heightened consciousness and longevity. Follow young Paul Atreides as he navigates the treacherous politics and power struggles of the feudal empire that controls Arrakis.",
     1, NULL},
    {"The Hitchhiker's Guide to the Galaxy",
     "Join Arthur Dent as he is whisked away from Earth before it is destroyed to make way for an intergalactic bypass. Along with his friend Ford Prefect and the totally unhelpful supercomputer, Deep Thought, they embark on a hilarious and absurd journey through space.",
     1, NULL}};

/**
 * @brief E_Reader入口函数，创建并启动StartupTask任务
 */
void E_Reader(void)
{
    // 创建并启动StartupTask任务
    xTaskCreate((TaskFunction_t)StartupTask, // 任务函数指针
                STARTUP_TASK_NAME,           // 任务名称
                STARTUP_TASK_STACK_SIZE,     // 任务栈大小
                (void *)NULL,                // 任务参数（在本示例中未使用）
                STARTUP_TASK_PRIORITY,       // 任务优先级
                (TaskHandle_t *)NULL);       // 任务句柄（在本示例中未使用）

    // 启动调度器，开始多任务调度
    vTaskStartScheduler();
}

/**
 * @brief 定时器回调函数，用于记录定时器是否触发
 * @param xTimer 定时器句柄
 */
void TimerCallback(TimerHandle_t xTimer)
{
    tick = true; // 将tick标志置为true
}

/**
 * @brief 打印当前书籍列表
 * @param book_sel 当前选中的书籍编号（从0开始）
 * @param books_num 书籍总数
 */
void PrintList(uint8_t book_sel, uint8_t books_num)
{
    char title[32];              // 声明书籍标题缓冲区
    static uint8_t listPage = 0; // 上次列表所在页码，在更新时需要用到

    // 这里不需要清除屏幕是因为未翻页文本可以完美覆盖
    GUI_SetFont(&FONT_TITLE);                                 // 设置标题字体
    GUI_DispStringHCenterAt("E-Reader", SCREEN_WIDTH / 2, 0); // 居中显示标题

    GUI_GotoXY(0, FONT_TITLE.YDist); // 定位列表起始坐标
    GUI_SetFont(&FONT_LIST);         // 设置列表字体

    if (book_sel / LIST_ROWS != listPage) // 若当前选中的书籍不在当前页码内，则更新列表
    {
        GUI_ClearRect(0, FONT_TITLE.YDist, SCREEN_WIDTH, SCREEN_HEIGHT - FONT_FOOTER.YDist); // 清除列表框，防止重绘时字体残留
        listPage = book_sel / LIST_ROWS;                                                     // 切换到当前书籍所在的页码
    }

    for (int i = 0; i < LIST_ROWS; i++) // 遍历当前书籍列表
    {
        int book_id = i + listPage * LIST_ROWS; // 当前书籍编号

        if (book_id >= books_num) // 若超过总书籍数
        {
            break; // 跳出循环
        }
        if (book_id == book_sel) // 若当前书籍为选中书籍
        {
            GUI_SetTextStyle(GUI_TS_UNDERLINE); // 设置下划线样式
        }
        sprintf(title, "%03d %s\n", book_id + 1, books[book_id].name); // 格式化输出标题
        GUI_DispString(title);                                         // 显示标题
        GUI_SetTextStyle(GUI_TS_NORMAL);                               // 恢复正常风格
    }

    GUI_SetFont(&FONT_FOOTER); // 设置页脚字体
    GUI_DispStringAt("</> 按键0/1-切换书籍 | 长按-快速切换 ", 0, SCREEN_HEIGHT - 2 * FONT_FOOTER.YDist);
    GUI_DispString("\n(X) 按键2-进入阅读 | 触摸按键-切换主题 ");
}

/**
 * @brief LCD任务，用于显示书籍菜单、阅读视图，以及对按键输入进行反馈
 * @param pvParameters 传入参数（未使用）
 */
void LcdTask(void *pvParameters)
{
    uint8_t book_sel = 0;                             // 当前选中书籍编号
    uint8_t books_num = sizeof(books) / sizeof(Book); // 书籍总数

    PrintList(book_sel, books_num); // 打印书籍列表

    while (true)
    {
        int key = key_scan(1);   // 扫描机械按键
        int tpad = tpad_scan(0); // 扫描触摸按键

        if (key || tpad)
        {
            BEEP(true); // 播放声音提示
        }

        switch (key) // 处理机械按键事件
        {
        case KEY0_PRES:
            if (book_sel > 0) // 如果当前不是第一本书，则向前选择
            {
                book_sel--; // 选择上一本书籍
                PrintList(book_sel, books_num);
            }
            break;

        case KEY1_PRES:
            if (book_sel < books_num - 1) // 如果当前不是最后一本书，则向后选择
            {
                book_sel++; // 选择下一本书籍
                PrintList(book_sel, books_num);
            }
            break;

        case WKUP_PRES:                     // 进入阅读视图
            ReadView(books + book_sel);     // 显示当前选中书籍的阅读视图
            GUI_Clear();                    // 返回时清除文本，以免残留
            PrintList(book_sel, books_num); // 打印更新后的书籍列表
            break;
        }

        if (tpad) // 处理触摸事件
        {
            GUI_SetBkColor(~GUI_GetBkColor()); // 切换背景色为与原来不同的色彩
            GUI_SetColor(~GUI_GetColor());     // 切换前景色为与原来不同的色彩
            GUI_Clear();                       // 确保所有已显示的元素都被完全移除
            PrintList(book_sel, books_num);    // 更新屏幕，重新打印书籍列表
        }

        BEEP(false); // 停止声音提示
    }
}

/**
 * @brief 打印指定字体和格式的书籍文本信息
 * @param font_cur 当前字体
 * @param rect     文本框矩形
 * @param book     书籍
 */
void PrintText(GUI_FONT *font_cur, GUI_RECT rect, Book *book)
{
    GUI_SetFont(font_cur);                             // 设置当前字体
    GUI_ClearRect(rect.x0, rect.y0, rect.x1, rect.y1); // 清除屏幕上之前显示的文本内容

    // 在指定矩形框内打印书籍内容，采用字符方式换行以保证单词不被拆开，对齐方式为左对齐
    GUI_DispStringInRectWrap(book->content, &rect, GUI_TA_LEFT, GUI_WRAPMODE_CHAR);
}

/**
 * @brief 打印指定字体和格式的书籍页脚信息（包括页码、字号、时间等）
 * @param font_cur 当前字体
 * @param rect     文本框矩形
 * @param page_cur 当前页码
 * @param pageNum  总页数
 */
void PrintFooter(GUI_FONT *font_cur, GUI_RECT rect, int page_cur, int pageNum)
{
    char footer[64];           // 页脚信息缓冲区
    GUI_SetFont(&FONT_FOOTER); // 设置页脚字体，避免缩放字体后导致页脚溢出

    // 将页码、总页数、当前字号等信息添加到页脚信息缓冲区中
    sprintf(footer, "</> 页码 %03d/%03d  (X) 字号 %d | 退出  ", page_cur, pageNum, font_cur->YSize);

    // 在文本框下方打印页脚信息（第一行）
    GUI_DispStringAt(footer, 0, rect.y1 + 1);

    // 将当前时间和温度信息添加到页脚信息缓冲区中
    sprintf(footer, "%02d:%02d:%02d %.2f", calendar.hour, calendar.min, calendar.sec, adc_get_temperature() / 100.0);

    // 在文本框下方打印页脚信息（第二行）
    GUI_DispString(footer);
}

/**
 * @brief 阅读视图：显示书籍内容、页码以及当前时间和温度等信息
 *
 * @param book 当前需要阅读的书籍
 */
void ReadView(Book *book)
{
    float progress;              // 阅读进度 (当前页码/页数)
    int page_cur = book->marker; // 当前页码

    /* 设置当前字体，如果没有保存的阅读字体则使用默认字体 */
    GUI_FONT *font_cur = // 当前字体
        book->font ? book->font : &GUI_FontChinese_16;
    GUI_RECT rect = // 文本框矩形位置
        {0, (1 - page_cur) * TEXTBOX_HEIGHT, SCREEN_WIDTH, TEXTBOX_HEIGHT - 1};
    int pageNum; // 该书籍总页数

    GUI_SetFont(font_cur); // 设定阅读字体，目的是保持同一字体时textheight不变
    // 每页行数 = 文本框高度/每行高度，总页数 = 文本总行数/每页行数
    pageNum = ceil(GUI_WrapGetNumLines(book->content, rect.x1, GUI_WRAPMODE_CHAR) / (TEXTBOX_HEIGHT / font_cur->YDist));

    PrintText(font_cur, rect, book);                // 打印文本内容
    PrintFooter(font_cur, rect, page_cur, pageNum); // 打印页脚信息

    while (true)
    {
        int key = key_scan(1);   // 扫描机械按键
        int tpad = tpad_scan(0); // 扫描触摸按键

        if (key || tpad) // 如果按键生效
        {
            BEEP(true); // 开启蜂鸣器来反馈用户操作状态
        }

        switch (key) // 检测并处理机械按键
        {
        case KEY0_PRES:
            if (page_cur > 1)
            {
                page_cur--;                                     // 当前页码减一
                rect.y0 += TEXTBOX_HEIGHT;                      // 更新文本框位置以显示当前页码内容
                PrintText(font_cur, rect, book);                // 打印文本内容
                PrintFooter(font_cur, rect, page_cur, pageNum); // 打印页脚信息
            }
            break;

        case KEY1_PRES:
            if (page_cur < pageNum)
            {
                page_cur++;                                     // 当前页码加一
                rect.y0 -= TEXTBOX_HEIGHT;                      // 更新文本框位置以显示当前页码内容
                PrintText(font_cur, rect, book);                // 打印文本内容
                PrintFooter(font_cur, rect, page_cur, pageNum); // 打印页脚信息
            }
            break;

        case WKUP_PRES:
            /* 把此时的书签和阅读字体保存到 Book 类型结构中 */
            book->marker = page_cur; // 保存此时的书签
            book->font = font_cur;   // 保存此时的阅读字体
            return;                  // 返回书籍列表

        default:
            break;
        }

        if (tpad) // 检测并处理触摸按键
        {
            switch (font_cur->YSize) // 检测当前字号
            {
            case 16:
                font_cur = &GUI_FontChinese_20;
                break;

            case 20:
                font_cur = &GUI_FontChinese_12;
                break;

            default:
                // 当字号为12时，缩放之后改用16号字体
                font_cur = &GUI_FontChinese_16;
                break;
            }

            progress = (float)page_cur / pageNum; // 计算当前阅读进度

            GUI_SetFont(font_cur); // 在调整字体前重新设置一遍
            pageNum = ceil(GUI_WrapGetNumLines(book->content, rect.x1, GUI_WRAPMODE_CHAR) / (TEXTBOX_HEIGHT / font_cur->YDist));
            page_cur = ceil(progress * pageNum);       // 根据阅读进度更新当前页码
            rect.y0 = (1 - page_cur) * TEXTBOX_HEIGHT; // 更新文本框位置以显示当前页码内容

            PrintText(font_cur, rect, book);                // 打印文本内容
            PrintFooter(font_cur, rect, page_cur, pageNum); // 打印页脚信息
        }

        else if (tick) // 检测定时器是否生效
        {
            static bool theme = false;            // 记录当前的主题（亮色/暗色）
            bool light = (lsens_get_val() >= 10); // 根据当前亮度传感器的状态确定光线强弱

            tick = false; // 重置定时器值（表示己经处理过）

            PrintFooter(font_cur, rect, page_cur, pageNum);

            if (theme != light) // 如果当前光线状态不同于主题
            {
                GUI_SetBkColor(~GUI_GetBkColor()); // 反转背景色
                GUI_SetColor(~GUI_GetColor());     // 反转前景色

                GUI_Clear();                                    // 以新的背景色覆盖屏幕
                PrintText(font_cur, rect, book);                // 打印文本内容
                PrintFooter(font_cur, rect, page_cur, pageNum); // 打印页脚信息

                theme = light; // 记录新状态
            }
        }

        BEEP(false); // 关闭蜂鸣器，输入事件已被处理完毕。
    }
}