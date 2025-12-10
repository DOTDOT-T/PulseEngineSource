#ifndef MATERIALAPI_H
#define MATERIALAPI_H

#include "Common/common.h"
#include "Common/dllExport.h"

class Material;
class Shader;

/**
 * @class MaterialApi
 * @brief High-level interface for runtime manipulation of material instances.
 *
 * MaterialApi exposes essential controls for altering the visual appearance  
 * of a renderable entity at runtime.  
 *
 * This includes shader assignment, color overrides, and other material-driven
 * visual parameters. It acts as the scripting-safe layer sitting on top of the
 * underlying Material resource.
 *
 * Typical use cases include:
 * - dynamic color feedback (damage flash, interaction highlight),
 * - runtime shader switching (stylized effects, special rendering passes),
 * - scripted visual transitions.
 */
class PULSE_ENGINE_DLL_API MaterialApi
{
private:
    /// Underlying material instance controlled by this API wrapper.
    Material* material;

public:

    /**
     * @brief Constructs a MaterialApi wrapper bound to an existing material.
     *
     * @param m Pointer to the material to expose to scripts or gameplay systems.
     */
    MaterialApi(Material* m);

    /**
     * @brief Retrieves the shader currently assigned to this material.
     *
     * @return Pointer to the active Shader object.
     */
    Shader* GetShader();

    /**
     * @brief Assigns a new shader to the material.
     *
     * The assignment takes effect immediately and alters the rendering
     * pipeline used for the associated object.
     *
     * @param shader Pointer to the shader that will replace the current one.
     */
    void SetShader(Shader* shader);

    /**
     * @brief Overrides the material's base color using RGB values.
     *
     * Values are expected in the [0.0f, 1.0f] range, and the change is applied
     * in real-time.  
     *
     * This is typically used for gameplay-driven color feedback, status effects,
     * or dynamic visual transitions.
     *
     * @param r Red channel.
     * @param g Green channel.
     * @param b Blue channel.
     */
    void ChangeColor(float r, float g, float b);
};

#endif
