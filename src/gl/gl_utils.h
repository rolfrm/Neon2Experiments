
u32 loadImage(u8 * pixels, u32 width, u32 height, u32 channels);
void gl_init_debug_calls();
u32 compileShaderFromFile(u32 gl_prog_type, const char * filepath);
u32 linkGlProgram(u32 shader_cnt, ...);
void glUniformVec2(u32 uniform, vec2 v);
void glUniformMat3(u32 uniform_location, mat3 mat);
