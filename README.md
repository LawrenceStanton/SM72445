# SM72445 Photovoltaic MPPT Controller Driver

## Overview

The SM72445 is an embedded photovoltaic MPPT controller, produced by Texas Instruments (and formerly National Semiconductor), and operates with a mixed analogue digital configurations interface. This driver provides a simple C++ object-orientated driver to allow for control and data reception from this device, and is agnostic to any embedded platform.

The design philosophy of this driver is comparatively unique in the embedded systems space. The object orientated approach results in a simple, scalable, and extensible driver, which is easy to use and understand. The driver is also designed to be agnostic to any embedded platform, and is therefore portable to any embedded system, with any I2C implementation (see [Design Patterns](#design-patterns)). The system also adopts strong unit testing with the [Google Test](https://google.github.io/googletest/) framework, allowing for a high degree of confidence in the driver's operation and ease of further development. CMake also allows for easy integration into parent projects.

## How to Use

1. Add this project as a `git submodule`.

    ```zsh
    git submodule add https://github.com/LawrenceStanton/SM72445 Modules/SM72445
    ```

2. Build with CMake by adding as a subdirectory.

    ```cmake
    # Add to Your Top-Level CMakeLists.txt
    add_subdirectory(Modules/HDC1080)
    # ...
    target_link_libraries(${YOUR_EXECUTABLE_NAME} PRIVATE 
        HDC1080::HDC1080
    )    
    ```

    > If you are not using CMake, you must then simply add the HDC1080 include directory and source files to your chosen build system.

3. Provide an I2C interface when constructing the driver. Refer to [Design Patterns](#design-patterns) below and [SM72445.hpp](Inc/SM72445.hpp) for more information.

4. Construct the SM72445 I2C interface and class object.

    ```cpp
    // Construct the I2C interface
    MyI2C i2cInterface(myParams);
    // Construct the SM72445 class object
    SM72445::SM72445 controller(&i2cInterface);
    ```

## Design Patterns

This driver follows an [Strategy Design Pattern](https://en.wikipedia.org/wiki/Strategy_pattern) with regards to the I2C communication. The driver defines an I2C interface (`SM72445::I2C`). The user must then provide a concrete implementation of this interface, and provide it to the driver class.

Often a concrete implementation will simply translate the I2C operations to the embedded platform's Hardware Abstraction Layer (HAL). For example, the [STM32Cube HAL](https://www.st.com/en/embedded-software/stm32cube-mcu-mpu-packages.html) provides an I2C interface, which can be used to implement the I2C operations. However, the user may also provide their own low level implementation, which may be useful in some applications, or a mocked implementation, which may be useful for testing purposes (see [Testing](#testing)).

> Take careful note of the API instructions when implementing the I2C interface, as the SM72445 itself is not well documented with regard to the I2C interface.

### Example

Below is an example of a typical declaration of the I2C interface.

```cpp
#include "SM72445.hpp"

class MyI2C : public SM72445::I2C {
public:
    MyI2C(MyI2CParams params) : params(params) {}
    virtual ~MyI2C() = default;

    virtual std::optional<SM72445::I2C::Register> read(SM72445::I2C::MemoryAddress address) override;
    virtual std::optional<SM72445::I2C::Register> write(SM72445::I2C::MemoryAddress address, SM72445::I2C::Register) override;
};
```

Refer to [Examples] for concrete examples of the I2C interface.

## Testing

This driver is designed to be tested using the [Google Test](https://google.github.io/googletest/) framework. The tests are located in the [Tests](Tests) directory, and can be run using CMake.

```zsh
mkdir build && cd build
cmake ..
cmake --build .
ctest
```

Given the limitations of many embedded systems; the tests are designed to be run on a host machine, rather than on the embedded platform itself. This is done by checking the `CMAKE_CROSSCOMPILING` variable, and if false, the tests are compiled for the host machine.

> If gtest and gmock are not installed on your system, CMake will attempt to download and build them automatically with FetchContent. Disable this behaviour by setting the CMake option `SM72445_AUTOFETCH_GTEST` to `OFF`.
>
> _Command Line:_
>
>```zsh
>cmake -DHDC1080_AUTOFETCH_GTEST=OFF
>```
>
> _Parent CMakeLists.txt:_
>
> ```cmake
> set(HDC1080_AUTOFETCH_GTEST OFF)
> # ...
> add_subdirectory(Modules/HDC1080)
>```

Run tests automatically by setting `test` as a build target.

The tests will be included in the parent build if ctest is also used there.
