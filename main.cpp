#include "IconsFontAwesome6.h"
#include "helpers.h"
#include "wildcatauth.h"
#include "wildcatmainwindow.h"
#include <SDL2/SDL.h>
#include <backends/imgui_impl_sdl2.h>
#include <backends/imgui_impl_sdlrenderer2.h>
#include <cstdlib>
#include <imgui.h>

int main() {
  SDL_Log("Starting Wildcat...");

  std::unique_ptr<WildcatAuth> auth = std::make_unique<WildcatAuth>();

  SDL_Log("Checking for user driver...");

  if (auth->isRoot()) {
    Helper_ErrorMsg("Don't run wildcat as root!\nSee the docs for more info");

    return EXIT_FAILURE;
  }

  if (std::getenv("WILDCAT_NO_DRIVER") == nullptr) {
    SDL_Log("Invoking user driver...");

    if (!auth->spawnAsRoot("./wildcatdriver")) {
      return EXIT_FAILURE;
    }
  }

  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) !=
      0) {
    printf("Error: %s\n", SDL_GetError());
    return -1;
  }

  SDL_Log("Creating window...");

  // Create window with SDL_Renderer graphics context
  SDL_WindowFlags window_flags =
      (SDL_WindowFlags)(SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
  SDL_Window *window = SDL_CreateWindow(
      "Wildcat - Uniden BC125AT programmer for Linux", SDL_WINDOWPOS_CENTERED,
      SDL_WINDOWPOS_CENTERED, 1280, 720, window_flags);

  if (window == nullptr) {
    printf("Error: SDL_CreateWindow(...): %s\n", SDL_GetError());
    return -1;
  }

  SDL_Renderer *renderer = SDL_CreateRenderer(
      window, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);
  if (renderer == nullptr) {
    SDL_Log("Failed to create SDL_Renderer!");
    return 0;
  }

  SDL_Log("Init: ImGui");

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();
  (void)io;
  io.ConfigFlags |=
      ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls

  // Setup fonts

  ImFont *font = nullptr;
  ImFont *iconFont = nullptr;

  float iconFontSize =
      18.f * 2.0f / 3.0f; // FontAwesome fonts need to have their sizes
  // reduced by 2.0f/3.0f in order to align correctly

  // merge in icons from Font Awesome
  static const ImWchar icon_ranges[] = {ICON_MIN_FA, ICON_MAX_FA, 0};
  ImFontConfig icons_config;
  icons_config.MergeMode = true;
  icons_config.PixelSnapH = true;
  icons_config.GlyphMinAdvanceX = iconFontSize;

#ifdef NDEBUG
  font = io.Fonts->AddFontFromFileTTF("resources/fonts/Inter-Regular.ttf", 18.f,
                                      nullptr, nullptr);

  iconFont = io.Fonts->AddFontFromFileTTF("resources/fonts/fa-solid-900.ttf",
                                          18.f, &icons_config, icon_ranges);
#endif

#ifndef NDEBUG // If we are in debug mode we are executing from the build dir
  font = io.Fonts->AddFontFromFileTTF("../resources/fonts/Inter-Regular.ttf",
                                      18.f, nullptr, nullptr);

  iconFont = io.Fonts->AddFontFromFileTTF("../resources/fonts/fa-solid-900.ttf",
                                          18.f, &icons_config, icon_ranges);
#endif

  io.FontDefault = font;

  // Setup Dear ImGui style
  // ImGui::StyleColorsDark();
  ImGui::StyleColorsLight();

  ImGui::GetStyle().WindowBorderSize = 0.0f;

  // Setup Platform/Renderer backends
  ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
  ImGui_ImplSDLRenderer2_Init(renderer);

  bool closed = false;

  ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

  WildcatMainWindow *wWindow = new WildcatMainWindow();

  while (!closed) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      ImGui_ImplSDL2_ProcessEvent(&event);
      if (event.type == SDL_QUIT)
        closed = true;
      if (event.type == SDL_WINDOWEVENT &&
          event.window.event == SDL_WINDOWEVENT_CLOSE &&
          event.window.windowID == SDL_GetWindowID(window))
        closed = true;
    }

    // Start the Dear ImGui frame
    ImGui_ImplSDLRenderer2_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();

    ImGui::PushFont(iconFont);

    wWindow->render();

    ImGui::PopFont();

    // Rendering
    ImGui::Render();
    SDL_RenderSetScale(renderer, io.DisplayFramebufferScale.x,
                       io.DisplayFramebufferScale.y);
    SDL_SetRenderDrawColor(
        renderer, (Uint8)(clear_color.x * 255), (Uint8)(clear_color.y * 255),
        (Uint8)(clear_color.z * 255), (Uint8)(clear_color.w * 255));
    SDL_RenderClear(renderer);
    ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), renderer);
    SDL_RenderPresent(renderer);
  }
}
