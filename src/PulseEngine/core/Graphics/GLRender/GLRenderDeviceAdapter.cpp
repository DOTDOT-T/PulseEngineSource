// File: GLRenderDeviceAdapter.cpp
#include "GLRender/GLRenderDeviceAdapter.h"
#include "GLCommandList.h"
#include <iostream>
#include <cassert>

// -- GLCommandList implementation --
GLCommandList::GLCommandList() 
{
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
}

GLCommandList::~GLCommandList() {}

void GLCommandList::Begin() { m_cmds.clear(); }
void GLCommandList::End() { /* nothing */ }

void GLCommandList::Clear() { m_cmds.clear(); }

void GLCommandList::ResourceBarrier(const ResourceBarrierDesc& desc)
{
    Cmd c; c.type = Cmd::Barrier; 
    m_cmds.push_back(std::move(c));
}

void GLCommandList::BindPipeline(PipelineHandle pso)
{
    Cmd c; c.type = Cmd::BindPipeline; 
    c.data.resize(sizeof(PipelineHandle));
    memcpy(c.data.data(), &pso, sizeof(PipelineHandle));
    m_cmds.push_back(std::move(c));
}

void GLCommandList::BindDescriptorSet(uint32_t slot, DescriptorSetHandle set)
{
    Cmd c; c.type = Cmd::BindPipeline; // reuse type for simple emulator
    c.data.resize(sizeof(DescriptorSetHandle) + sizeof(uint32_t));
    memcpy(c.data.data(), &slot, sizeof(uint32_t));
    memcpy(c.data.data() + sizeof(uint32_t), &set, sizeof(DescriptorSetHandle));
    m_cmds.push_back(std::move(c));
}

void GLCommandList::BindVertexBuffer(BufferHandle buffer, uint64_t offset)
{
    Cmd c; c.type = Cmd::BindVB; c.data.resize(sizeof(BufferHandle) + sizeof(uint64_t));
    memcpy(c.data.data(), &buffer, sizeof(BufferHandle));
    memcpy(c.data.data()+sizeof(BufferHandle), &offset, sizeof(uint64_t));
    m_cmds.push_back(std::move(c));
}

void GLCommandList::BindIndexBuffer(BufferHandle buffer, uint64_t offset)
{
    Cmd c; c.type = Cmd::BindIB; c.data.resize(sizeof(BufferHandle) + sizeof(uint64_t));
    memcpy(c.data.data(), &buffer, sizeof(BufferHandle));
    memcpy(c.data.data()+sizeof(BufferHandle), &offset, sizeof(uint64_t));
    m_cmds.push_back(std::move(c));
}

void GLCommandList::DrawIndexed(uint32_t indexCount, uint32_t instanceCount,
                                uint32_t firstIndex, int32_t vertexOffset,
                                uint32_t firstInstance)
{
    Cmd c; c.type = Cmd::DrawIndexed; c.data.resize(sizeof(uint32_t)*6);
    uint32_t* p = reinterpret_cast<uint32_t*>(c.data.data());
    p[0] = indexCount; p[1] = instanceCount; p[2] = firstIndex; p[3] = static_cast<uint32_t>(vertexOffset); p[4] = firstInstance; p[5] = vao;
    m_cmds.push_back(std::move(c));
}

void GLCommandList::Dispatch(uint32_t x, uint32_t y, uint32_t z)
{
    Cmd c; c.type = Cmd::Dispatch; c.data.resize(sizeof(uint32_t)*3);
    uint32_t* p = reinterpret_cast<uint32_t*>(c.data.data()); p[0]=x; p[1]=y; p[2]=z;
    m_cmds.push_back(std::move(c));
}

void GLCommandList::CopyBuffer(BufferHandle dst, uint64_t dstOffset,
                               BufferHandle src, uint64_t srcOffset,
                               size_t size)
{
    Cmd c; c.type = Cmd::CopyBuffer; c.data.resize(sizeof(BufferHandle)*2 + sizeof(uint64_t)*2 + sizeof(size_t));
    uint8_t* ptr = c.data.data();
    memcpy(ptr, &dst, sizeof(BufferHandle)); ptr += sizeof(BufferHandle);
    memcpy(ptr, &dstOffset, sizeof(uint64_t)); ptr += sizeof(uint64_t);
    memcpy(ptr, &src, sizeof(BufferHandle)); ptr += sizeof(BufferHandle);
    memcpy(ptr, &srcOffset, sizeof(uint64_t)); ptr += sizeof(uint64_t);
    memcpy(ptr, &size, sizeof(size_t));
    m_cmds.push_back(std::move(c));
}

// -- GLRenderDeviceAdapter implementation --
GLRenderDeviceAdapter::GLRenderDeviceAdapter() {}
GLRenderDeviceAdapter::~GLRenderDeviceAdapter() { Shutdown(); }

bool GLRenderDeviceAdapter::Initialize(const char* appName, int width, int height)
{
    if (!glfwInit()) {
        std::cerr << "GLFW init failed\n";
        return false;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    m_width = width; m_height = height;
    m_window = glfwCreateWindow(width, height, appName, nullptr, nullptr);
    if (!m_window) {
        std::cerr << "Failed to create GLFW window\n";
        glfwTerminate();
        return false;
    }

    glfwMakeContextCurrent(m_window);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to load GLAD\n";
        return false;
    }

    glViewport(0,0,width,height);
    m_startTime = glfwGetTime();
    return true;
}

void GLRenderDeviceAdapter::Shutdown()
{
    m_commandLists.clear();
    // destroy GL resources
    for (auto &kv : m_buffers) glDeleteBuffers(1, &kv.second.glId);
    for (auto &kv : m_textures) glDeleteTextures(1, &kv.second.glId);
    for (auto &kv : m_pipelines) if (kv.second.program) glDeleteProgram(kv.second.program);

    m_buffers.clear(); m_textures.clear(); m_pipelines.clear();

    if (m_window) { glfwDestroyWindow(m_window); m_window = nullptr; }
    glfwTerminate();
}

void GLRenderDeviceAdapter::Resize(uint32_t width, uint32_t height)
{
    m_width = width; m_height = height;
    if (m_window) glfwSetWindowSize(m_window, width, height);
    glViewport(0,0,width,height);
}

void GLRenderDeviceAdapter::BeginFrame() 
{ 
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
void GLRenderDeviceAdapter::EndFrame() { /* flush if needed */ }

void GLRenderDeviceAdapter::Present()
{
    if (m_window) {
        glfwSwapBuffers(m_window);
        glfwPollEvents();
    }
}

BufferHandle GLRenderDeviceAdapter::CreateBuffer(const GpuBufferDesc& desc, const void* initialData)
{
    GLuint glId = CreateGLBuffer(desc, initialData);
    BufferHandle h = m_nextBufferHandle++;
    m_buffers[h] = { glId, desc };
    return h;
}

void GLRenderDeviceAdapter::DestroyBuffer(BufferHandle h)
{
    auto it = m_buffers.find(h);
    if (it == m_buffers.end()) return;
    glDeleteBuffers(1, &it->second.glId);
    m_buffers.erase(it);
}

TextureHandle GLRenderDeviceAdapter::CreateTexture(const GpuTextureDesc& desc)
{
    GLuint glId = CreateGLTexture(desc);
    TextureHandle h = m_nextTextureHandle++;
    m_textures[h] = { glId, desc };
    return h;
}

void GLRenderDeviceAdapter::DestroyTexture(TextureHandle h)
{
    auto it = m_textures.find(h);
    if (it == m_textures.end()) return;
    glDeleteTextures(1, &it->second.glId);
    m_textures.erase(it);
}

PipelineHandle GLRenderDeviceAdapter::CreatePipeline(const PipelineDesc& desc)
{
    PipelineHandle h = m_nextPipelineHandle++;
    GLuint program = CompileAndLinkShaders(desc);
    m_pipelines[h] = { desc, program };
    return h;
}

void GLRenderDeviceAdapter::DestroyPipeline(PipelineHandle h)
{
    auto it = m_pipelines.find(h);
    if (it == m_pipelines.end()) return;
    if (it->second.program) glDeleteProgram(it->second.program);
    m_pipelines.erase(it);
}

DescriptorLayoutHandle GLRenderDeviceAdapter::CreateDescriptorSetLayout(const DescriptorSetLayoutDesc& desc)
{
    DescriptorLayoutHandle h = m_nextDescriptorLayoutHandle++;
    m_layouts[h] = { desc };
    return h;
}

DescriptorSetHandle GLRenderDeviceAdapter::AllocateDescriptorSet(DescriptorLayoutHandle layout)
{
    DescriptorSetHandle h = m_nextDescriptorSetHandle++;
    m_sets[h] = {};
    return h;
}

void GLRenderDeviceAdapter::UpdateDescriptorSet(DescriptorSetHandle set, const DescriptorSetDesc& desc)
{
    auto it = m_sets.find(set);
    if (it == m_sets.end()) return;
    it->second.desc = desc;
}

CommandListHandle GLRenderDeviceAdapter::CreateCommandList()
{
    CommandListHandle h = m_nextCommandListHandle++;
    m_commandLists[h] = std::make_unique<GLCommandList>();
    return h;
}

ICommandList* GLRenderDeviceAdapter::GetCommandList(CommandListHandle h)
{
    auto it = m_commandLists.find(h);
    if (it == m_commandLists.end()) return nullptr;
    return it->second.get();
}

CommandQueueHandle GLRenderDeviceAdapter::GetGraphicsQueue() { return m_graphicsQueue; }

void GLRenderDeviceAdapter::Submit(CommandQueueHandle queue, CommandListHandle list, FenceHandle fenceToSignal)
{
    auto it = m_commandLists.find(list);
    if (it == m_commandLists.end()) return;

    // Replay recorded commands immediately on GL context
    const auto& cmds = it->second->GetRecorded();
    for (const auto& c : cmds) {
        switch (c.type) {
            case GLCommandList::Cmd::BindPipeline: {
                PipelineHandle ph; memcpy(&ph, c.data.data(), sizeof(PipelineHandle));
                auto pit = m_pipelines.find(ph);
                if (pit != m_pipelines.end()) glUseProgram(pit->second.program);
                break;
            }
            case GLCommandList::Cmd::BindVB: {
                BufferHandle bh; uint64_t offset; 
                memcpy(&bh, c.data.data(), sizeof(BufferHandle)); 
                memcpy(&offset, c.data.data()+sizeof(BufferHandle), sizeof(uint64_t));
                auto bit = m_buffers.find(bh);
                if (bit != m_buffers.end()) {
                    glBindBuffer(GL_ARRAY_BUFFER, bit->second.glId);
                    glEnableVertexAttribArray(0);
                    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);

                    glEnableVertexAttribArray(1);
                    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(3 * sizeof(float)));
                }
                break;
            }
            case GLCommandList::Cmd::BindIB: {
                BufferHandle bh; uint64_t offset; memcpy(&bh, c.data.data(), sizeof(BufferHandle)); memcpy(&offset, c.data.data()+sizeof(BufferHandle), sizeof(uint64_t));
                auto bit = m_buffers.find(bh);
                if (bit != m_buffers.end()) glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bit->second.glId);
                break;
            }
            case GLCommandList::Cmd::DrawIndexed: {
                const uint32_t* p = reinterpret_cast<const uint32_t*>(c.data.data());
                uint32_t indexCount = p[0];
                glBindVertexArray(p[5]);
                glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, nullptr);
                break;
            }
            case GLCommandList::Cmd::Dispatch: {
                // GL compute
                uint32_t* p = reinterpret_cast<uint32_t*>(const_cast<uint8_t*>(c.data.data()));
                glDispatchCompute(p[0], p[1], p[2]);
                break;
            }
            case GLCommandList::Cmd::CopyBuffer: {
                // simple CPU fallback copy
                BufferHandle dst; uint64_t dstOff; BufferHandle src; uint64_t srcOff; size_t size;
                const uint8_t* ptr = c.data.data();
                memcpy(&dst, ptr, sizeof(BufferHandle)); ptr+=sizeof(BufferHandle);
                memcpy(&dstOff, ptr, sizeof(uint64_t)); ptr+=sizeof(uint64_t);
                memcpy(&src, ptr, sizeof(BufferHandle)); ptr+=sizeof(BufferHandle);
                memcpy(&srcOff, ptr, sizeof(uint64_t)); ptr+=sizeof(uint64_t);
                memcpy(&size, ptr, sizeof(size_t));
                // naive path: map both buffers (if cpu-writable)
                auto sit = m_buffers.find(src);
                auto dit = m_buffers.find(dst);
                if (sit!=m_buffers.end() && dit!=m_buffers.end()) {
                    if (sit->second.desc.cpuWritable && dit->second.desc.cpuWritable) {
                        // Not implemented: need to keep shadow copies or glGetBufferSubData which is slow
                    }
                }
                break;
            }
            case GLCommandList::Cmd::Barrier: {
                // GL has implicit memory visibility for many cases; nothing to do.
                break;
            }
        }
    }

    // clear commands after submit to emulate single-use command lists
    it->second->Clear();
}

void GLRenderDeviceAdapter::WaitForFence(FenceHandle fence, uint64_t value) { /* stub */ }

float GLRenderDeviceAdapter::GetTimeSeconds() const { return static_cast<float>(glfwGetTime() - m_startTime); }

// -- helpers --
GLuint GLRenderDeviceAdapter::CreateGLBuffer(const GpuBufferDesc& desc, const void* data)
{
    GLuint id; glGenBuffers(1, &id);
    GLenum target = GL_ARRAY_BUFFER;
    if (desc.usage == GpuBufferDesc::Usage::Index) target = GL_ELEMENT_ARRAY_BUFFER;
    else if (desc.usage == GpuBufferDesc::Usage::Uniform) target = GL_UNIFORM_BUFFER;

    glBindBuffer(target, id);
    GLenum usage = desc.cpuWritable ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW;
    glBufferData(target, desc.size, data, usage);
    glBindBuffer(target, 0);
    return id;
}

GLuint GLRenderDeviceAdapter::CreateGLTexture(const GpuTextureDesc& desc)
{
    GLuint id; glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);
    GLint internal = GL_RGBA8;
    GLenum format = GL_RGBA; GLenum type = GL_UNSIGNED_BYTE;
    if (desc.format == GpuTextureDesc::Format::RGBA16F) { internal = GL_RGBA16F; type = GL_HALF_FLOAT; }
    if (desc.format == GpuTextureDesc::Format::Depth32F) { internal = GL_DEPTH_COMPONENT32F; format = GL_DEPTH_COMPONENT; type = GL_FLOAT; }

    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        internal,
        desc.width,
        desc.height,
        0,
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        nullptr
    );
    // default params
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_2D, 0);
    return id;
}

GLuint GLRenderDeviceAdapter::CompileAndLinkShaders(const PipelineDesc& desc)
{
    const char* vsSrc = R"(
#version 450 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aColor;

out vec3 vColor;

void main() {
    gl_Position = vec4(aPos, 1.0);
    vColor = aColor;
}

    )";

    const char* fsSrc = R"(
#version 450 core
in vec3 vColor;
out vec4 FragColor;

void main() {
    FragColor = vec4(vColor, 1.0);
}

    )";

    auto compile = [](GLenum type, const char* src)->GLuint{
        GLuint s = glCreateShader(type);
        glShaderSource(s,1,&src,nullptr);
        glCompileShader(s);
        GLint ok; glGetShaderiv(s, GL_COMPILE_STATUS, &ok);
        if (!ok) {
            char buf[1024]; glGetShaderInfoLog(s, 1024, nullptr, buf);
            std::cerr << "Shader compile error: " << buf << "\n";
        }
        return s;
    };

    GLuint vs = compile(GL_VERTEX_SHADER, vsSrc);
    GLuint fs = compile(GL_FRAGMENT_SHADER, fsSrc);
    GLuint prog = glCreateProgram();
    glAttachShader(prog, vs); glAttachShader(prog, fs);
    glLinkProgram(prog);
    GLint ok; glGetProgramiv(prog, GL_LINK_STATUS, &ok);
    if (!ok) { char buf[1024]; glGetProgramInfoLog(prog,1024,nullptr,buf); std::cerr << "Prog link: "<<buf<<"\n"; }
    glDetachShader(prog, vs); glDetachShader(prog, fs);
    glDeleteShader(vs); glDeleteShader(fs);
    return prog;
}

std::string GLRenderDeviceAdapter::ReadShaderSource(uint32_t shaderHandle) { return std::string(); }
