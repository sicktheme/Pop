#include "Engine.h"

char Level_01[ALevel::Level_Height][ALevel::Level_Width] = {
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

ABall::ABall() : Ball_X_Pos(20), Ball_Y_Pos(170), Ball_Speed(3.0), Ball_Direction(M_PI - M_PI_4)
{

}

AsPlatform::AsPlatform() : Inner_Width(21), X_Pos(AsBorder::Border_X_Offset), X_Step(AsEngine::Global_Scale * 2), Width(28)
{

}

AsEngine::AsEngine()
{
   
}

void ABall::Init() {
    AsEngine::Create_Pen_Brush(111, 57, 255, Ball_Pen, Ball_Brush);
}

void ABall::Draw(HDC hdc, RECT& paint_area, AsEngine *engine) {

    RECT intersection_rect;

    if (!IntersectRect(&intersection_rect, &paint_area, &Ball_Rect))
        return;

    // 1. ������� ����
    SelectObject(hdc, engine->BG_Pen);
    SelectObject(hdc, engine->BG_Brush);

    Ellipse(hdc, Prev_Ball_Rect.left, Prev_Ball_Rect.top, Prev_Ball_Rect.right - 1, Prev_Ball_Rect.bottom - 1);

    // 2. ��������� ������
    SelectObject(hdc, Ball_Pen);
    SelectObject(hdc, Ball_Brush);

    Ellipse(hdc, Ball_Rect.left, Ball_Rect.top, Ball_Rect.right - 1, Ball_Rect.bottom - 1);
}

void ABall::Move(AsEngine *engine, ALevel *level, AsPlatform* platform) {

    int next_x_pos, next_y_pos;
    int max_x_pos = AsEngine::Max_X_Pos - Ball_Size;
    int Y_Pos = AsPlatform::Y_Pos - Ball_Size;

    Prev_Ball_Rect = Ball_Rect;

    next_x_pos = Ball_X_Pos + (int)(Ball_Speed * cos(Ball_Direction));
    next_y_pos = Ball_Y_Pos - (int)(Ball_Speed * sin(Ball_Direction));

    // ��������� ������� ��� ��������� �� �����
    if (next_x_pos < AsBorder::Border_X_Offset) {
        next_x_pos = ALevel::Level_X_Offset - (next_x_pos - ALevel::Level_X_Offset);
        Ball_Direction = M_PI - Ball_Direction;
    }

    if (next_y_pos < AsBorder::Border_Y_Offset) {
        next_y_pos = AsBorder::Border_Y_Offset - (next_y_pos - AsBorder::Border_Y_Offset);
        Ball_Direction = -Ball_Direction;
    }

    if (next_x_pos > AsEngine::Max_X_Pos - Ball_Size) {
        next_x_pos = max_x_pos - (next_x_pos - max_x_pos);
        Ball_Direction = M_PI - Ball_Direction;
    }

    if (next_y_pos > AsEngine::Max_Y_Pos) {
        next_y_pos = AsEngine::Max_Y_Pos - (next_y_pos - AsEngine::Max_Y_Pos);
        Ball_Direction = M_PI + (M_PI - Ball_Direction);
    }

    // ��������� ������� ��� ��������� �� ��������� 
    if (next_y_pos > Y_Pos) {
        if (next_x_pos >= platform->X_Pos && next_x_pos <= platform->X_Pos + platform->Width) {
            next_y_pos = Y_Pos - (next_y_pos - Y_Pos);
            Ball_Direction = M_PI + (M_PI - Ball_Direction);
        }
    }

    level->Check_Level_Brick_Hit(next_y_pos, Ball_Direction);

    // �������� ������
    Ball_X_Pos = next_x_pos;
    Ball_Y_Pos = next_y_pos;

    Ball_Rect.left = Ball_X_Pos * AsEngine::Global_Scale;
    Ball_Rect.top = Ball_Y_Pos * AsEngine::Global_Scale;
    Ball_Rect.right = Ball_Rect.left + Ball_Size * AsEngine::Global_Scale;
    Ball_Rect.bottom = Ball_Rect.top + Ball_Size * AsEngine::Global_Scale;

    InvalidateRect(engine->Hwnd, &Prev_Ball_Rect, FALSE);
    InvalidateRect(engine->Hwnd, &Ball_Rect, FALSE);
}

void ALevel::Init() {

    Letter_Pen = CreatePen(PS_SOLID, AsEngine::Global_Scale, RGB(255, 255, 255));

    AsEngine::Create_Pen_Brush(85, 255, 255, Brick_Blue_Pen, Brick_Blue_Brush);
    AsEngine::Create_Pen_Brush(255, 85, 85, Brick_Red_Pen, Brick_Red_Brush);

    Level_Rect.left = ALevel::Level_X_Offset * AsEngine::Global_Scale;
    Level_Rect.top = ALevel::Level_Y_Offset * AsEngine::Global_Scale;
    Level_Rect.right = Level_Rect.left + ALevel::Cell_Width * ALevel::Level_Width * AsEngine::Global_Scale;
    Level_Rect.bottom = Level_Rect.top + ALevel::Cell_Height * ALevel::Level_Height * AsEngine::Global_Scale;
}

void ALevel::Check_Level_Brick_Hit(int& next_y_pos, double &ball_direction)
{
    int brick_y_pos = Level_Y_Offset + Level_Height * Cell_Height;

    // ��������� ������� ��� ��������� �� ��������
    for (int i = Level_Height - 1; i >= 0; i--) {
        for (int j = 0; j < Level_Width; j++) {
            if (Level_01[i][j] == 0)
                continue;

            if (next_y_pos < brick_y_pos) {
                next_y_pos = brick_y_pos - (next_y_pos - brick_y_pos);
                ball_direction = -ball_direction;
            }
        }

        brick_y_pos -= Cell_Height;
    }
}

void ALevel::Redrew_Platform_Brick(HDC hdc, int x, int y, EBrick_Type is_blue) {
    // ����� �������

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

    RoundRect(hdc, x * AsEngine::Global_Scale, y * AsEngine::Global_Scale, (x + Brick_Width) * AsEngine::Global_Scale, (y + Brick_Height) * AsEngine::Global_Scale, 2 * AsEngine::Global_Scale, 2 * AsEngine::Global_Scale);
}

void ALevel::Set_Brick_Letter_Colors(bool is_switch_color, HPEN& front_pen, HBRUSH& front_brush, HPEN& back_pen, HBRUSH& back_brush) {

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

void ALevel::Redrew_Platform_Brick_Letter(HDC hdc, int x, int y, EBrick_Type brick_type, ELetter_Type letter_type, int rotation_step) {
    // ��������� ����� 

    bool switch_color;
    int brick_half_height = Brick_Height * AsEngine::Global_Scale / 2;
    int back_part_offset;
    double offset;
    double rotation_angle; // �������������� ���� � ���� ��������
    HPEN front_pen, back_pen;
    HBRUSH front_brush, back_brush;
    XFORM xform, old_xform;

    if (!(brick_type == EBT_Blue || brick_type == EBT_Red))
        return;

    // ������������� ���� �������� � ���� ��������
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
        // ����� ����

        SelectObject(hdc, back_pen);
        SelectObject(hdc, back_brush);

        Rectangle(hdc, x, y + brick_half_height - AsEngine::Global_Scale, x + Brick_Width * AsEngine::Global_Scale, y + brick_half_height);

        // ����� ��������� �����
        SelectObject(hdc, front_pen);
        SelectObject(hdc, front_brush);

        Rectangle(hdc, x, y + brick_half_height, x + Brick_Width * AsEngine::Global_Scale, y + brick_half_height + AsEngine::Global_Scale - 1);
    }
    else {

        SetGraphicsMode(hdc, GM_ADVANCED);

        // ��������� ������� ���������� �����
        xform.eM11 = 1.0f;
        xform.eM12 = 0.0f;
        xform.eM21 = 0.0f;
        xform.eM22 = (float)cos(rotation_angle);
        xform.eDx = (float)x;
        xform.eDy = (float)y + (float)(brick_half_height);
        GetWorldTransform(hdc, &old_xform);
        SetWorldTransform(hdc, &xform);

        // ����� ����
        SelectObject(hdc, back_pen);
        SelectObject(hdc, back_brush);

        offset = 3.0 * (1.0 - fabs(xform.eM22)) * (double)AsEngine::Global_Scale;
        back_part_offset = (int)round(offset);

        Rectangle(hdc, 0, -brick_half_height - back_part_offset, Brick_Width * AsEngine::Global_Scale, brick_half_height - back_part_offset);

        // ����� ��������� �����
        SelectObject(hdc, front_pen);
        SelectObject(hdc, front_brush);

        Rectangle(hdc, 0, -brick_half_height, Brick_Width * AsEngine::Global_Scale, brick_half_height);


        if (rotation_step > 4 && rotation_step <= 12) {
            if (letter_type == ELT_O) {
                SelectObject(hdc, Letter_Pen);
                Ellipse(hdc, 0 + 5 * AsEngine::Global_Scale, (-5 * AsEngine::Global_Scale) / 2, 0 + 10 * AsEngine::Global_Scale, 5 * AsEngine::Global_Scale / 2);
            }
        }

        SetWorldTransform(hdc, &old_xform);
    }
}

void ALevel::Draw(HDC hdc, RECT& paint_area) {
    // ��������� �������� ������

    RECT intersection_rect;

    if (!IntersectRect(&intersection_rect, &paint_area, &Level_Rect))
        return;

    for (int i = 0; i < Level_Height; i++)
        for (int j = 0; j < Level_Width; j++)
            Redrew_Platform_Brick(hdc, Level_X_Offset + j * Cell_Width, Level_Y_Offset + i * Cell_Height, (EBrick_Type)Level_01[i][j]);
}

void AsPlatform::Init() {
    Highlight_Pen = CreatePen(PS_SOLID, 0, RGB(255, 255, 255));

    AsEngine::Create_Pen_Brush(7, 254, 118, Platform_Circle_Pen, Platform_Circle_Brush);
    AsEngine::Create_Pen_Brush(241, 254, 197, Platform_Inner_Pen, Platform_Inner_Brush);
}

void AsPlatform::Redraw_Platform(AsEngine *engine) {
    // ����������� ���������

    Prev_Platform_Rect = Platform_Rect;

    Platform_Rect.left = X_Pos * AsEngine::Global_Scale;
    Platform_Rect.top = Y_Pos * AsEngine::Global_Scale;
    Platform_Rect.right = Platform_Rect.left + Width * AsEngine::Global_Scale;
    Platform_Rect.bottom = Platform_Rect.top + Height * AsEngine::Global_Scale;

    InvalidateRect(engine->Hwnd, &Prev_Platform_Rect, FALSE);
    InvalidateRect(engine->Hwnd, &Platform_Rect, FALSE);
}

void AsPlatform::Draw(HDC hdc, AsEngine* engine, RECT& paint_area) {
    // ��������� ���������

    int x = X_Pos;
    int y = Y_Pos;

    RECT intersection_rect;

    if (!IntersectRect(&intersection_rect, &paint_area, &Platform_Rect))
        return;

    SelectObject(hdc, engine->BG_Pen);
    SelectObject(hdc, engine->BG_Brush);
    Rectangle(hdc, Prev_Platform_Rect.left, Prev_Platform_Rect.top, Prev_Platform_Rect.right, Prev_Platform_Rect.bottom);

    // 1. ��������� ������� �������
    SelectObject(hdc, Platform_Circle_Pen);
    SelectObject(hdc, Platform_Circle_Brush);

    Ellipse(hdc, x * AsEngine::Global_Scale, y * AsEngine::Global_Scale, (x + Circle_Size) * AsEngine::Global_Scale, (y + Circle_Size) * AsEngine::Global_Scale);
    Ellipse(hdc, (x + Inner_Width) * AsEngine::Global_Scale, y * AsEngine::Global_Scale, (x + Circle_Size + Inner_Width) * AsEngine::Global_Scale,
        (y + Circle_Size) * AsEngine::Global_Scale);

    // 2. ��������� �����
    SelectObject(hdc, Highlight_Pen);

    Arc(hdc, (x + 1) * AsEngine::Global_Scale, (y + 1) * AsEngine::Global_Scale, (x + Circle_Size - 1) * AsEngine::Global_Scale,
        (y + Circle_Size - 1) * AsEngine::Global_Scale, (x + 1 + 1) * AsEngine::Global_Scale, (y + 1) * AsEngine::Global_Scale,
        (x + 1) * AsEngine::Global_Scale, (y + 1 + 2) * AsEngine::Global_Scale);

    // 3. ��������� ������� �����
    SelectObject(hdc, Platform_Inner_Pen);
    SelectObject(hdc, Platform_Inner_Brush);

    RoundRect(hdc, (x + 4) * AsEngine::Global_Scale, (y + 1) * AsEngine::Global_Scale, (x + 4 + Inner_Width - 1) * AsEngine::Global_Scale, (y + 1 + 5) * AsEngine::Global_Scale,
        3 * AsEngine::Global_Scale, 3 * AsEngine::Global_Scale);
}

void AsBorder::Init() {
    AsEngine::Create_Pen_Brush(85, 255, 255, Border_Blue_Pen, Border_Blue_Brush);
    AsEngine::Create_Pen_Brush(255, 255, 255, Border_White_Pen, Border_White_Brush);
}

void AsBorder::Redrew_Platform_Element(HDC hdc, int x, int y, bool top_border, AsEngine* engine) {
    // �������� ����� ������
    SelectObject(hdc, Border_Blue_Pen);
    SelectObject(hdc, Border_Blue_Brush);

    if (top_border)
        Rectangle(hdc, x * AsEngine::Global_Scale, (y + 1) * AsEngine::Global_Scale, (x + 4) * AsEngine::Global_Scale, (y + 4) * AsEngine::Global_Scale);
    else
        Rectangle(hdc, (x + 1) * AsEngine::Global_Scale, y * AsEngine::Global_Scale, (x + 4) * AsEngine::Global_Scale, (y + 4) * AsEngine::Global_Scale);

    // ����� ����� ������
    SelectObject(hdc, Border_White_Pen);
    SelectObject(hdc, Border_White_Brush);

    if (top_border)
        Rectangle(hdc, x * AsEngine::Global_Scale, y * AsEngine::Global_Scale, (x + 4) * AsEngine::Global_Scale, (y + 1) * AsEngine::Global_Scale);
    else
        Rectangle(hdc, x * AsEngine::Global_Scale, y * AsEngine::Global_Scale, (x + 1) * AsEngine::Global_Scale, (y + 4) * AsEngine::Global_Scale);

    // ����� � ������
    SelectObject(hdc, engine->BG_Pen);
    SelectObject(hdc, engine->BG_Brush);

    if (top_border)
        Rectangle(hdc, (x + 2) * AsEngine::Global_Scale, (y + 2) * AsEngine::Global_Scale, (x + 3) * AsEngine::Global_Scale, (y + 3) * AsEngine::Global_Scale);
    else
        Rectangle(hdc, (x + 2) * AsEngine::Global_Scale, (y + 1) * AsEngine::Global_Scale, (x + 3) * AsEngine::Global_Scale, (y + 2) * AsEngine::Global_Scale);
}

void AsBorder::Draw(HDC hdc, RECT& paint_area, AsEngine* engine) {
    // ��������� �����

    // 1. ����� �����
    for (int i = 0; i < 50; i++)
        Redrew_Platform_Element(hdc, 2, 1 + i * 4, false, engine);

    // 2. ����� ������
    for (int i = 0; i < 50; i++)
        Redrew_Platform_Element(hdc, 201, 1 + i * 4, false, engine);

    // 3. ����� ������
    for (int i = 0; i < 50; i++)
        Redrew_Platform_Element(hdc, 3 + i * 4, 0, true, engine);
}

void AsEngine::Init_Engine(HWND hWnd) {
    // �������������

    Hwnd = hWnd;

    Create_Pen_Brush(39, 71, 71, BG_Pen, BG_Brush);

    Ball.Init();
    Level.Init();
    Platform.Init();
    Border.Init();

    Platform.Redraw_Platform(this);

    SetTimer(Hwnd, Timer_ID, 50, NULL);
}

void AsEngine::Redrew_Platform_Frame(HDC hdc, RECT& paint_area) {
    // ��������� ������

    Level.Draw(hdc, paint_area);
    Platform.Draw(hdc, this, paint_area);
    Ball.Draw(hdc, paint_area, this);
    Border.Draw(hdc, paint_area, this);
}

int AsEngine::On_Key_Down(EKey_Type key_type) {

    switch (key_type) {
    case EKT_Left:
        Platform.X_Pos -= Platform.X_Step;

        if (Platform.X_Pos <= AsBorder::Border_X_Offset)
            Platform.X_Pos = AsBorder::Border_X_Offset;

        Platform.Redraw_Platform(this);
        break;

    case EKT_Right:
        Platform.X_Pos += Platform.X_Step;

        if (Platform.X_Pos >= Max_X_Pos - Platform.Width + 1)
            Platform.X_Pos = Max_X_Pos - Platform.Width + 1;

        Platform.Redraw_Platform(this);
        break;

    case EKT_Space:
        break;
    }

    return 0;
}

int AsEngine::On_Timer() {

    Ball.Move(this, &Level, &Platform);

    return 0;
}

void AsEngine::Create_Pen_Brush(unsigned char r, unsigned char g, unsigned char b, HPEN& pen, HBRUSH& brush) {
    // �������� ����� � ��������� 

    pen = CreatePen(PS_SOLID, 0, RGB(r, g, b));
    brush = CreateSolidBrush(RGB(r, g, b));
}
