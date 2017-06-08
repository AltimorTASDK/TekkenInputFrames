#include <Windows.h>

extern "C" void cmd_store_hook();
extern "C" void cmd_reset_hook();
extern "C" void hud_postrender_hook();

extern "C" int cmd_frame[40];

struct FVector2D
{
	float X, Y;
	FVector2D(float X, float Y) : X(X), Y(Y) {}
};

struct FLinearColor
{
	float R, G, B, A;
	FLinearColor(float R, float G, float B, float A) : R(R), G(G), B(B), A(A) {}
};

struct FString
{
	int ArrayNum, ArrayMax;
	void *AllocatorInstance;
};

struct UFont;
struct UCanvas;
struct AHud;

extern "C" void draw_frames(AHud *hud)
{
	// Input display enabled
	if (*(char*)(0x1432B5C30) == 0)
		return;

	const auto game_mode = *(char**)(0x14334ABB0) + 0x460;
	const auto idk_lol = (*(void*(__thiscall**)(void*))(*(char**)(game_mode) + 0x10))(game_mode);
	const auto player_idx = *(int*)((char*)(idk_lol) + 0x14);
	const auto input_obj = ((void**)(0x143364920))[player_idx];
	if (input_obj == nullptr)
		return;

	const auto cmd_idx = *(int*)((char*)(input_obj) + 0x14D8);

	const auto canvas = *(UCanvas**)((char*)(hud) + 0x3C8);
	const auto width = *(float*)((char*)(canvas) + 0x30);
	const auto height = *(float*)((char*)(canvas) + 0x34);

	// GEngine::SmallFont
	const auto font = ((UFont*(*)())(0x14168B420))();

	for (auto i = 0; i < 39; i++)
	{
		const auto idx = (cmd_idx - i - 2 + 40) % 40;
		const auto next_idx = (cmd_idx - i - 1 + 40) % 40;

		if (cmd_frame[idx] == 0)
			break;

		const auto frames = min(999, cmd_frame[next_idx] - cmd_frame[idx]);

		FString text;
		const auto format_string = (void(__fastcall*)(FString*, const wchar_t*, ...))(0x140665990);
		format_string(&text, L"%i", frames);

		// UCanvas::K2_DrawText
		const auto K2_DrawText = (void(__fastcall*)(
			UCanvas*,
			UFont*,
			FString*,
			FVector2D,
			FLinearColor&,
			float,
			FLinearColor&,
			FVector2D,
			bool,
			bool,
			bool,
			FLinearColor&))(0x1416C35A0);

		K2_DrawText(
			canvas,
			font,
			&text,
			FVector2D((1683.F - 38.F * i) * (width / 1920.F), 787.F * (height / 1080.F)),
			FLinearColor(1.F, 1.F, 1.F, 1.F),
			0.F,
			FLinearColor(0.F, 0.F, 0.F, 0.F),
			FVector2D(0.F, 0.F),
			true,
			false,
			true,
			FLinearColor(0.F, 0.F, 0.F, 1.F));
	}
}

void insert_jmp(uintptr_t addr, void *dest)
{
	constexpr auto patch_size = 12;

	DWORD old_protect;
	VirtualProtect((void*)(addr), patch_size, PAGE_EXECUTE_READWRITE, &old_protect);

	*(WORD*)(addr) = 0xB848; // mov rax, dest
	*(void**)(addr + 2) = dest;
	*(WORD*)(addr + 10) = 0xE0FF; // jmp rax

	VirtualProtect((void*)(addr), patch_size, old_protect, &old_protect);
}

BOOL WINAPI DllMain(HINSTANCE inst, DWORD reason, void *reserved)
{
	if (reason != DLL_PROCESS_ATTACH)
		return false;

	insert_jmp(0x140612AED, cmd_store_hook);
	insert_jmp(0x14062C720, cmd_reset_hook);
	//insert_jmp(0x1403E25B0, cmd_draw_hook);
	insert_jmp(0x1413E25F0, hud_postrender_hook);

	return true;
}