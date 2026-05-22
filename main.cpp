// Provem funcoes e estruturas e "enumerators"
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

// Reportar erros
#include <iostream>
#include <stdexcept>

// dá os macros EXIT_SUCESS e EXIT_FAILURE
#include <cstdlib>

#include <cstring>
#include <vector>

class HelloTriangleApplication {
public:
    void run() {
        // adicionando objetos privados
        // iniciar janela
        initWindow();
        // adiciona funcoes do vulkan
        initVulkan();
        // cria os frames (frame loops) ate fechar o programa
        mainLoop();
        // limpar tudo na saida do programa
        cleanup();
    }

private:
    GLFWwindow* window; // cria o objeto janela

    const uint32_t WIDTH = 800; // largura da janela
    const uint32_t HEIGHT = 600; // altura da janela

    const std::vector<const char*> validationLayers = {
        "VK_LAYER_KHRONOS_validation"
    };

    // especificar se as camadas devem ou nao ser ativadas
    // com base se é release ou debug
    #ifdef NDEBUG //"not debug"
    const bool enableValidationLayers = false;
    #else
    const bool enableValidationLayers = true;
    #endif

    VkInstance instance;

    void initVulkan() {
        // cria uma instancia do Vulkan
        createInstance();
    }

    bool checkValidationLayerSupport() {
        uint32_t layerCount;
        vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

        std::vector<VkLayerProperties> availableLayers(layerCount);
        vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());


        // checa se todas as camadas de validacao existem e estao disponiveis
        for (const char* layerName : validationLayers) {
            bool layerFound = false;

            for (const auto& layerProperties : availableLayers) {
                if (strcmp(layerName, layerProperties.layerName) == 0) {
                    layerFound = true;
                    break;
                }
            }

            if (!layerFound) {
                return false;
            }
        }

        return true;
    }

    void createInstance() {
        if (enableValidationLayers && !checkValidationLayerSupport()) {
            throw std::runtime_error("Camadas de validacao inqueridas, mas nao disponiveis!");
        }

        // isso aqui e tecnicamente opcional
        // mas envia informacoes uteis ao driver
        // para a otimizacao do programa
        VkApplicationInfo appInfo{};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = "Hello Triangle";
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.pEngineName = "No Engine";
        appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.apiVersion = VK_API_VERSION_1_0;

        // varias informacoes do vulkan sao passadas em structs
        // entao aqui vai mais alguns dados para criar a instancia
        VkInstanceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &appInfo;

        // numero de extensoes e as extensoes
        // vulkan e uma api agnostica (multi linguagem/ plataforma)
        // entao precisa de extensoes para intermediar com o sistema de janelas
        uint32_t glfwExtensionCount = 0;
        const char** glfwExtensions;
        glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

        createInfo.enabledExtensionCount = glfwExtensionCount;
        createInfo.ppEnabledExtensionNames = glfwExtensions;

        createInfo.enabledLayerCount = 0;

        if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {
            throw std::runtime_error("falha ao criar uma instancia!");
        }
    }

    void initWindow() {
        // inicia a biblioteca
        glfwInit();

        // o glfw cria janelas em Vulkan e em OpenGL
        // precisa tornar explicito que quero apenas o vulkan
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

        // cria a janela
        window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);
    }

    void mainLoop() {

        // se a janela nao deve fechar
        // seja por erros ou por input do usuario
        // ela nao vai
        while (!glfwWindowShouldClose(window)) {
            glfwPollEvents();
        }

    }

    // faz uma limpeza na hora de sair/fechar
    void cleanup() {

        glfwDestroyWindow(window);

        vkDestroyInstance(instance, nullptr);

        glfwTerminate();
    }
};

int main() {
    HelloTriangleApplication app;

    try {
        app.run();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}