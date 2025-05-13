#ifndef UI_REGISTRATION_C
#define UI_REGISTRATION_C

#include "clay.h"
#include "ui_base.c"

Clay_RenderCommandArray
RenderRegistrationForm(ClayVideoDemo_Data *data) {
    data->arena_frame->pos = 0;

    Clay_BeginLayout();

    Clay_Sizing layoutExpand = {
        .width = CLAY_SIZING_GROW(0),
        .height = CLAY_SIZING_GROW(0)
    };

    Clay_Color backgroundColor = {43, 41, 51, 255};
    Clay_Color contentBackgroundColor = {90, 90, 90, 255};
    Clay_Color inputBackgroundColor = {70, 70, 70, 255};
    Clay_Color buttonColor = {120, 90, 210, 255};
    Clay_Color buttonHoverColor = {140, 110, 230, 255};
    Clay_Color textColor = {255, 255, 255, 255};
    Clay_Color placeholderColor = {200, 200, 200, 180};

    // Form container
    CLAY({ 
        .id = CLAY_ID("RegistrationFormContainer"),
        .backgroundColor = backgroundColor,
        .layout = {
            .layoutDirection = CLAY_TOP_TO_BOTTOM,
            .sizing = layoutExpand,
            .padding = CLAY_PADDING_ALL(16),
            .childGap = 16
        },
        .cornerRadius = CLAY_CORNER_RADIUS(12)
    }) {
        // Form title
        CLAY({
            .layout = {
                .sizing = {
                    .width = CLAY_SIZING_GROW(0)
                },
                .padding = CLAY_PADDING_ALL(8)
            }
        }) {
            CLAY({ .layout = { .sizing = { .width = CLAY_SIZING_GROW(0) }}}) {}
            CLAY_TEXT(CLAY_STRING("Create an Account"), CLAY_TEXT_CONFIG({
                .fontId = FONT_ID_BODY_16,
                .fontSize = 28,
                .textColor = textColor
            }));
            CLAY({ .layout = { .sizing = { .width = CLAY_SIZING_GROW(0) }}}) {}
        }

        // Username field
        CLAY({
            .id = CLAY_ID("UsernameContainer"),
            .layout = {
                .layoutDirection = CLAY_TOP_TO_BOTTOM,
                .sizing = {
                    .width = CLAY_SIZING_GROW(0)
                },
                .childGap = 8
            }
        }) {
            CLAY_TEXT(CLAY_STRING("Username"), CLAY_TEXT_CONFIG({
                .fontId = FONT_ID_BODY_16,
                .fontSize = 16,
                .textColor = textColor
            }));

            CLAY({
                .id = CLAY_ID("UsernameInput"),
                .backgroundColor = inputBackgroundColor,
                .cornerRadius = CLAY_CORNER_RADIUS(6),
                .layout = {
                    .sizing = {
                        .width = CLAY_SIZING_FIXED(200),
                        .height = CLAY_SIZING_GROW(0)
                    },
                    .padding = CLAY_PADDING_ALL(12)
                }
            }) {
                CLAY_TEXT(CLAY_STRING("Enter your username"), CLAY_TEXT_CONFIG({
                    .fontId = FONT_ID_BODY_16,
                    .fontSize = 16,
                    .textColor = placeholderColor
                }));
            }
        }

        // Email field
        CLAY({
            .id = CLAY_ID("EmailContainer"),
            .layout = {
                .layoutDirection = CLAY_TOP_TO_BOTTOM,
                .sizing = {
                    .width = CLAY_SIZING_GROW(0)
                },
                .childGap = 8
            }
        }) {
            CLAY_TEXT(CLAY_STRING("Email Address"), CLAY_TEXT_CONFIG({
                .fontId = FONT_ID_BODY_16,
                .fontSize = 16,
                .textColor = textColor
            }));

            CLAY({
                .id = CLAY_ID("EmailInput"),
                .backgroundColor = inputBackgroundColor,
                .cornerRadius = CLAY_CORNER_RADIUS(6),
                .layout = {
                    .sizing = {
                        .width = CLAY_SIZING_FIXED(200),
                        .height = CLAY_SIZING_GROW(0)
                    },
                    .padding = CLAY_PADDING_ALL(12)
                }
            }) {
                CLAY_TEXT(CLAY_STRING("Enter your email"), CLAY_TEXT_CONFIG({
                    .fontId = FONT_ID_BODY_16,
                    .fontSize = 16,
                    .textColor = placeholderColor
                }));
            }
        }

        // Password field
        CLAY({
            .id = CLAY_ID("PasswordContainer"),
            .layout = {
                .layoutDirection = CLAY_TOP_TO_BOTTOM,
                .sizing = {
                    .width = CLAY_SIZING_GROW(0)
                },
                .childGap = 8
            }
        }) {
            CLAY_TEXT(CLAY_STRING("Password"), CLAY_TEXT_CONFIG({
                .fontId = FONT_ID_BODY_16,
                .fontSize = 16,
                .textColor = textColor
            }));

            CLAY({
                .id = CLAY_ID("PasswordInput"),
                .backgroundColor = inputBackgroundColor,
                .cornerRadius = CLAY_CORNER_RADIUS(6),
                .layout = {
                    .sizing = {
                        .width = CLAY_SIZING_FIXED(200),
                        .height = CLAY_SIZING_GROW(0)
                    },
                    .padding = CLAY_PADDING_ALL(12)
                }
            }) {
                CLAY_TEXT(CLAY_STRING("••••••••"), CLAY_TEXT_CONFIG({
                    .fontId = FONT_ID_BODY_16,
                    .fontSize = 16,
                    .textColor = placeholderColor
                }));
            }
        }

        // Confirm Password field
        CLAY({
            .id = CLAY_ID("ConfirmPasswordContainer"),
            .layout = {
                .layoutDirection = CLAY_TOP_TO_BOTTOM,
                .sizing = {
                    .width = CLAY_SIZING_GROW(0)
                },
                .childGap = 8
            }
        }) {
            CLAY_TEXT(CLAY_STRING("Confirm Password"), CLAY_TEXT_CONFIG({
                .fontId = FONT_ID_BODY_16,
                .fontSize = 16,
                .textColor = textColor
            }));

            CLAY({
                .id = CLAY_ID("ConfirmPasswordInput"),
                .backgroundColor = inputBackgroundColor,
                .cornerRadius = CLAY_CORNER_RADIUS(6),
                .layout = {
                    .sizing = {
                        .width = CLAY_SIZING_GROW(0),
                        .height = CLAY_SIZING_FIXED(48)
                    },
                    .padding = CLAY_PADDING_ALL(12)
                }
            }) {
                CLAY_TEXT(CLAY_STRING("••••••••"), CLAY_TEXT_CONFIG({
                    .fontId = FONT_ID_BODY_16,
                    .fontSize = 16,
                    .textColor = placeholderColor
                }));
            }
        }

        // Terms and Conditions checkbox
        CLAY({
            .id = CLAY_ID("TermsContainer"),
            .layout = {
                .sizing = {
                    .width = CLAY_SIZING_GROW(0)
                },
                .childGap = 12
            }
        }) {
            CLAY({
                .id = CLAY_ID("CheckboxContainer"),
                .layout = {
                    .childGap = 16,
                    .childAlignment = {
                        .y = CLAY_ALIGN_Y_CENTER
                    }
                }
            }) {
                // Checkbox
                CLAY({
                    .id = CLAY_ID("Checkbox"),
                    .backgroundColor = inputBackgroundColor,
                    .cornerRadius = CLAY_CORNER_RADIUS(4),
                    .layout = {
                        .sizing = {
                            .width = CLAY_SIZING_FIXED(24),
                            .height = CLAY_SIZING_FIXED(24)
                        }
                    }
                }) {}

                // Terms text
                CLAY({
                    .layout = {
                        .sizing = {
                            .width = CLAY_SIZING_GROW(0)
                        }
                    }
                }) {
                    CLAY_TEXT(CLAY_STRING("I agree to the Terms and Privacy Policy"), CLAY_TEXT_CONFIG({
                        .fontId = FONT_ID_BODY_16,
                        .fontSize = 14,
                        .textColor = textColor
                    }));
                }
            }
        }

        // Register button with hover effect
        CLAY({
            .id = CLAY_ID("RegisterButton"),
            .backgroundColor = Clay_Hovered() ? buttonHoverColor : buttonColor,
            .cornerRadius = CLAY_CORNER_RADIUS(8),
            .layout = {
                .sizing = {
                    .width = CLAY_SIZING_GROW(0),
                    .height = CLAY_SIZING_FIXED(56)
                },
                .padding = CLAY_PADDING_ALL(16),
                .childAlignment = {
                    .x = CLAY_ALIGN_X_CENTER,
                    .y = CLAY_ALIGN_Y_CENTER
                }
            }
        }) {
            CLAY_TEXT(CLAY_STRING("Create Account"), CLAY_TEXT_CONFIG({
                .fontId = FONT_ID_BODY_16,
                .fontSize = 18,
                .textColor = textColor,
            }));
        }

        // Already have an account section
        CLAY({
            .id = CLAY_ID("LoginLinkContainer"),
            .layout = {
                .sizing = {
                    .width = CLAY_SIZING_GROW(0)
                },
                .childAlignment = {
                    .x = CLAY_ALIGN_X_CENTER
                },
                .padding = {8, 8, 0, 0}
            }
        }) {
            CLAY({
                .layout = {
                    .childGap = 8
                }
            }) {
                CLAY_TEXT(CLAY_STRING("Already have an account?"), CLAY_TEXT_CONFIG({
                    .fontId = FONT_ID_BODY_16,
                    .fontSize = 14,
                    .textColor = textColor
                }));

                CLAY({
                    .id = CLAY_ID("LoginLink"),
                    .backgroundColor = Clay_Hovered() ? (Clay_Color){120, 120, 120, 30} : (Clay_Color){0, 0, 0, 0},
                    .cornerRadius = CLAY_CORNER_RADIUS(4),
                    .layout = {
                        .padding = {4, 4, 4, 4}
                    }
                }) {
                    CLAY_TEXT(CLAY_STRING("Sign In"), CLAY_TEXT_CONFIG({
                        .fontId = FONT_ID_BODY_16,
                        .fontSize = 14,
                        .textColor = (Clay_Color){140, 170, 255, 255}
                    }));
                }
            }
        }
    }

    Clay_RenderCommandArray renderCommands = Clay_EndLayout();
    for (int32_t i = 0; i < renderCommands.length; i++) {
        Clay_RenderCommandArray_Get(&renderCommands, i)->boundingBox.y += data->yOffset;
    }
    return renderCommands;
}

#endif // UI_REGISTRATION_C
