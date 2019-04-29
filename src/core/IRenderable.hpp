#pragma once

/**
 * A base class for objects that can be rendered in the render queue pipeline, generally mesh objects.
 */
class IRenderable {
public:
    virtual void render() const = 0;
};