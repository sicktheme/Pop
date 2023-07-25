#include "Engine.h"

#define _USE_MATH_DEFINES
#include <math.h>

HWND Hwnd;
HPEN Highlight_Pen, Letter_Pen, BG_Pen, Brick_Red_Pen, Brick_Blue_Pen, Platform_Circle_Pen, Platform_Inner_Pen;
HBRUSH Brick_Red_Brush, Brick_Blue_Brush, BG_Brush, Platform_Circle_Brush, Platform_Inner_Brush;
RECT Platform_Rect, Prev_Platform_Rect;
RECT Level_Rect;

const int Global_Scale = 3;
const int Brick_Width = 15;
const int Brick_Height = 7;
const int Cell_Width = 16;
const int Cell_Height = 8;
const int Level_X_Offset = 8;
const int Level_Y_Offset = 6;
const int Level_Width = 14;
const int Level_Height = 12;
const int Circle_Size = 7;
const int Platform_Y_Pos = 185;
const int Platform_Height = 7;

int Inner_Width = 21;
int Platform_X_Pos = 0;
int Platform_X_Step = Global_Scale * 2;
int Platform_Width = 28;


enum ELetter_Type {
    ELT_None,
    ELT_O
};

enum EBrick_Type {
    EBT_None,
    EBT_Red,
    EBT_Blue
};

char Level_01[Level_Width][Level_Height] = {
    0,0,0,0,0,0,0,0,0,0,0,0,
    1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,
    2,2,2,2,2,2,2,2,2,2,2,2,
    2,2,2,2,2,2,2,2,2,2,2,2,
    1,1,1,1,1,1,1,1,1,1,1,1,
    1,1,1,1,1,1,1,1,1,1,1,1,
    2,2,2,2,2,2,2,2,2,2,2,2,
    2,2,2,2,2,2,2,2,2,2,2,2,
    0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0
};

void Create_Pen_Brush(unsigned char r, unsigned char g, unsigned char b, HPEN& pen, HBRUSH& brush) {
    // Создание кисти и карандаша 

    pen = CreatePen(PS_SOLID, 0, RGB(r, g, b));
    brush = CreateSolidBrush(RGB(r, g, b));
}

void Redraw_Platform() {
    // Перерисовка платформы

    Prev_Platform_Rect = Platform_Rect;

    Platform_Rect.left = (Level_X_Offset + Platform_X_Pos) * Global_Scale;
    Platform_Rect.top = Platform_Y_Pos * Global_Scale;
    Platform_Rect.right = Platform_Rect.left + Platform_Width * Global_Scale;
    Platform_Rect.bottom = Platform_Rect.top + Platform_Height * Global_Scale;

    InvalidateRect(Hwnd, &Prev_Platform_Rect, FALSE);
    InvalidateRect(Hwnd, &Platform_Rect, FALSE);
}

void Init_Engine(HWND hWnd) {
    // Инициализация

    Hwnd = hWnd;

    Highlight_Pen = CreatePen(PS_SOLID, 0, RGB(255, 255, 255));
    Letter_Pen = CreatePen(PS_SOLID, Global_Scale, RGB(255, 255, 255));

    Create_Pen_Brush(39, 71, 71, BG_Pen, BG_Brush);
    Create_Pen_Brush(85, 255, 255, Brick_Blue_Pen, Brick_Blue_Brush);
    Create_Pen_Brush(255, 85, 85, Brick_Red_Pen, Brick_Red_Brush);
    Create_Pen_Brush(7, 254, 118, Platform_Circle_Pen, Platform_Circle_Brush);
    Create_Pen_Brush(241, 254, 197, Platform_Inner_Pen, Platform_Inner_Brush);

    Level_Rect.left = Level_X_Offset * Global_Scale;
    Level_Rect.top = Level_Y_Offset * Global_Scale;
    Level_Rect.right = Level_Rect.left + Cell_Width * Level_Width * Global_Scale;
    Level_Rect.bottom = Level_Rect.top + Cell_Height * Level_Height * Global_Scale;

    Redraw_Platform();
}

void Draw_Brick(HDC hdc, int x, int y, EBrick_Type is_blue) {
    // Вывод кирпича

    HPEN pen;
    HBRUSH brush;

    switch (is_blue) {
    case EBT_None:
        return;

    case EBT_Blue:
        pen = Brick_Blue_Pen;
        brush = Brick_Blue_Brush;
        break;

    case EBT_Red:
        pen = Brick_Red_Pen;
        brush = Brick_Red_Brush;
        break;

    default:
        return;
    }

    SelectObject(hdc, pen);
    SelectObject(hdc, brush);

    RoundRect(hdc, x * Global_Scale, y * Global_Scale, (x + Brick_Width) * Global_Scale, (y + Brick_Height) * Global_Scale, 2 * Global_Scale, 2 * Global_Scale);
}

void Set_Brick_Letter_Colors(bool is_switch_color, HPEN& front_pen, HBRUSH& front_brush, HPEN& back_pen, HBRUSH& back_brush) {

    if (is_switch_color) {
        front_pen = Brick_Red_Pen;
        front_brush = Brick_Red_Brush;

        back_pen = Brick_Blue_Pen;
        back_brush = Brick_Blue_Brush;
    }
    else {
        front_pen = Brick_Blue_Pen;
        front_brush = Brick_Blue_Brush;

        back_pen = Brick_Red_Pen;
        back_brush = Brick_Red_Brush;
    }
}

void Draw_Brick_Letter(HDC hdc, int x, int y, EBrick_Type brick_type, ELetter_Type letter_type, int rotation_step) {
    // Отрисовка буквы 

    bool switch_color;
    int brick_half_height = Brick_Height * Global_Scale / 2;
    int back_part_offset;
    double offset;
    double rotation_angle; // Преобразование шага в угол поворота
    HPEN front_pen, back_pen;
    HBRUSH front_brush, back_brush;
    XFORM xform, old_xform;

    if (!(brick_type == EBT_Blue || brick_type == EBT_Red))
        return;

    // Корректировка шага вращения и угла поворота
    rotation_step = rotation_step % 16;

    if (rotation_step < 8)
        rotation_angle = 2.0 * M_PI / 16.0 * (double)rotation_step;
    else
        rotation_angle = 2.0 * M_PI / 16.0 * (double)(8L - (long long)rotation_step);

    if (rotation_step > 4 && rotation_step <= 12) {

        if (brick_type == EBT_Blue)
            switch_color = true;
        else
            switch_color = false;
    }

    else {
        if (brick_type == EBT_Red)
            switch_color = true;
        else
            switch_color = false;
    }

    Set_Brick_Letter_Colors(switch_color, front_pen, front_brush, back_pen, back_brush);

    if (rotation_step == 4 || rotation_step == 12) {
        // Вывод фона

        SelectObject(hdc, back_pen);
        SelectObject(hdc, back_brush);

        Rectangle(hdc, x, y + brick_half_height - Global_Scale, x + Brick_Width * Global_Scale, y + brick_half_height);

        // Вывод переднего плана
        SelectObject(hdc, front_pen);
        SelectObject(hdc, front_brush);

        Rectangle(hdc, x, y + brick_half_height, x + Brick_Width * Global_Scale, y + brick_half_height + Global_Scale - 1);
    }
    else {

        SetGraphicsMode(hdc, GM_ADVANCED);

        // Настройка матрицы переворота буквы
        xform.eM11 = 1.0f;
        xform.eM12 = 0.0f;
        xform.eM21 = 0.0f;
        xform.eM22 = (float)cos(rotation_angle);
        xform.eDx = (float)x;
        xform.eDy = (float)y + (float)(brick_half_height);
        GetWorldTransform(hdc, &old_xform);
        SetWorldTransform(hdc, &xform);

        // Вывод фона
        SelectObject(hdc, back_pen);
        SelectObject(hdc, back_brush);

        offset = 3.0 * (1.0 - fabs(xform.eM22)) * (double)Global_Scale;
        back_part_offset = (int)round(offset);

        Rectangle(hdc, 0, -brick_half_height - back_part_offset, Brick_Width * Global_Scale, brick_half_height - back_part_offset);

        // Вывод переднего плана
        SelectObject(hdc, front_pen);
        SelectObject(hdc, front_brush);

        Rectangle(hdc, 0, -brick_half_height, Brick_Width * Global_Scale, brick_half_height);


        if (rotation_step > 4 && rotation_step <= 12) {
            if (letter_type == ELT_O) {
                SelectObject(hdc, Letter_Pen);
                Ellipse(hdc, 0 + 5 * Global_Scale, (-5 * Global_Scale) / 2, 0 + 10 * Global_Scale, 5 * Global_Scale / 2);
            }
        }

        SetWorldTransform(hdc, &old_xform);
    }
}

void Draw_Level(HDC hdc) {
    // Отрисовка кирпичей уровня

    for (int i = 0; i < 14; i++)
        for (int j = 0; j < 12; j++)
            Draw_Brick(hdc, Level_X_Offset + j * Cell_Width, Level_Y_Offset + i * Cell_Height, (EBrick_Type)Level_01[i][j]);
}

void Draw_Platform(HDC hdc, int x, int y) {
    // Отрисовка платформы

    SelectObject(hdc, BG_Pen);
    SelectObject(hdc, BG_Brush);
    Rectangle(hdc, Prev_Platform_Rect.left, Prev_Platform_Rect.top, Prev_Platform_Rect.right, Prev_Platform_Rect.bottom);

    // 1. Отрисовка боковых шариков
    SelectObject(hdc, Platform_Circle_Pen);
    SelectObject(hdc, Platform_Circle_Brush);

    Ellipse(hdc, x * Global_Scale, y * Global_Scale, (x + Circle_Size) * Global_Scale, (y + Circle_Size) * Global_Scale);
    Ellipse(hdc, (x + Inner_Width) * Global_Scale, y * Global_Scale, (x + Circle_Size + Inner_Width) * Global_Scale,
        (y + Circle_Size) * Global_Scale);

    // 2. Отрисовка блика
    SelectObject(hdc, Highlight_Pen);

    Arc(hdc, (x + 1) * Global_Scale, (y + 1) * Global_Scale, (x + Circle_Size - 1) * Global_Scale,
        (y + Circle_Size - 1) * Global_Scale, (x + 1 + 1) * Global_Scale, (y + 1) * Global_Scale,
        (x + 1) * Global_Scale, (y + 1 + 2) * Global_Scale);

    // 3. Отрисовка средней части
    SelectObject(hdc, Platform_Inner_Pen);
    SelectObject(hdc, Platform_Inner_Brush);

    RoundRect(hdc, (x + 4) * Global_Scale, (y + 1) * Global_Scale, (x + 4 + Inner_Width - 1) * Global_Scale, (y + 1 + 5) * Global_Scale,
        3 * Global_Scale, 3 * Global_Scale);
}

void Draw_Frame(HDC hdc, RECT& paint_area) {
    // Отрисовка экрана

    RECT intersection_rect;

    if (IntersectRect(&intersection_rect, &paint_area, &Level_Rect))
        Draw_Level(hdc);

    if(IntersectRect(&intersection_rect, &paint_area, &Platform_Rect))
        Draw_Platform(hdc, Level_X_Offset + Platform_X_Pos, Platform_Y_Pos);

    //for (int i = 0; i < Cell_Width; i++) {
    //    Draw_Brick_Letter(hdc, 20 + i * Cell_Width * Global_Scale, 100, EBT_Blue, ELT_O, i);
    //    Draw_Brick_Letter(hdc, 20 + i * Cell_Width * Global_Scale, 130, EBT_Red, ELT_O, i);
    //}
}

int On_Key_Down(EKey_Type key_type) {
    
    switch (key_type) {
    case EKT_Left:
        Platform_X_Pos -= Platform_X_Step;
        Redraw_Platform();
        break;

    case EKT_Right:
        Platform_X_Pos += Platform_X_Step;
        Redraw_Platform();
        break;

    case EKT_Space:
        break;
    }

    return 0;
}