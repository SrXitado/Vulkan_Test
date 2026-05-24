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

// ele faz uma variavel nao conter nenhum tipo de valor até atribuir algo a ela
#include <optional>


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

// cuida dos callbacks de debug
// essa funcao nao roda automaticamente pois é uma funcao de extensao
// agora ela vai rodar de fundo
VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if (func != nullptr) {
        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    } else {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

// destruidor do mensageiro
void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) {
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr) {
        func(instance, debugMessenger, pAllocator);
    }
}

struct QueueFamilyIndices {
    std::optional<uint32_t> graphicsFamily;

    bool isComplete() {
        return graphicsFamily.has_value();
    }
};


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

    VkInstance instance;    // declara a variavel da instancia
    VkDebugUtilsMessengerEXT debugMessenger;    // variavel do mensageiro de debug

    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    VkDevice device;

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
    void initVulkan() {
        // cria uma instancia do Vulkan
        createInstance();
        setupDebugMessenger();
        pickPhysicalDevice(); // seleciona a placa de video
        //createLogicalDevice(); // cria um dispositivo logico
    }

    void mainLoop() {

        // se a janela nao deve fechar
        // seja por erros ou por input do usuario
        // ela nao vai
        while (!glfwWindowShouldClose(window)) {
            glfwPollEvents();
        }

    }

    // dá inicio ao mensageiro de debug
    void setupDebugMessenger() {
        if (!enableValidationLayers) return;

        VkDebugUtilsMessengerCreateInfoEXT createInfo;
        populateDebugMessengerCreateInfo(createInfo);

        if (CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &debugMessenger) != VK_SUCCESS) {
            throw std::runtime_error("falha ao criar um mensageiro de debug!");
        }
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
                if (strcmp(layerName, layerProperties.layerName) == 0) { // compara a string com os nomes
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

    // essa e uma funcao de "debug callback"
    // sempre que der erro, essa funcao é chamada e retorna os dados do erro
    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,     // a gravidade da mensagem/bandeiras de aviso
    VkDebugUtilsMessageTypeFlagsEXT messageType,                // o tipo da mensagem
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,  // contem detalhes da mensagem
    void* pUserData) {

        std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

        return VK_FALSE;
    }

    // essa funcao vai retornar a lista de extensoes obrigatorias baseada
    // se as camdas de validacao estao ativas ou nao
    std::vector<const char*> getRequiredExtensions() {
        uint32_t glfwExtensionCount = 0;
        const char** glfwExtensions;
        glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

        std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

        if (enableValidationLayers) {
            extensions.push_back("VK_EXT_debug_utils");
        }

        return extensions;
    }
    void createInstance() {
        // checa se vai ou nao ativar as camadas de validacao
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

        // vulkan e uma api agnostica (multi linguagem/ plataforma)
        // entao precisa de extensoes para intermediar com o sistema de janelas
        auto extensions = getRequiredExtensions();
        createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
        createInfo.ppEnabledExtensionNames = extensions.data();

        createInfo.enabledLayerCount = 0;

        // faz debug da criacao e destruicao da instancia
        // faz debug da criacao e destruicao da instancia
        VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
        if (enableValidationLayers) {
            createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
            createInfo.ppEnabledLayerNames = validationLayers.data();

            populateDebugMessengerCreateInfo(debugCreateInfo);
            createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*) &debugCreateInfo;
        } else {
            createInfo.enabledLayerCount = 0;

            createInfo.pNext = nullptr;
        }

        // apenas checa se foi possivel criar uma instancia, senão ele cria um erro
        if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {
            throw std::runtime_error("falha ao criar uma instancia!");
        }
    }

    // faz debug da criacao e destruicao
    void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo) {
        createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        createInfo.pfnUserCallback = debugCallback;
    }

    // basicamente ele seleciona a placa de video que sera utilizada
    void pickPhysicalDevice() {
        uint32_t deviceCount = 0;
        vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
        if (deviceCount == 0) {
            throw std::runtime_error("falhou em achar GPUs com suporte a Vulkan!");
        }

        // array para alocar todos os VkPhysicalDevice/GPU

        std::vector<VkPhysicalDevice> devices(deviceCount);
        vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

        for (const auto& device : devices) {
            if (isDeviceSuitable(device)) {
                physicalDevice = device;
                break;
            }
        }

        // checa de a GPU é nula/nao existe
        if (physicalDevice == VK_NULL_HANDLE) {
            throw std::runtime_error("failed to find a suitable GPU!");
        }
    }

    // poderia adicionar algo como um benchmark para determinar qual GPU é mais eficiente
    // porem isso demandaria muito tempo e geralmente as pessoas tem 1 GPU (integrada ou extena)
    // então retorno "true" aceitando qualquer placa de video
    bool isDeviceSuitable(VkPhysicalDevice device) {
        QueueFamilyIndices indices = findQueueFamilies(device);

        return indices.isComplete();
    }

    // acha a familia das filas
    QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device) {
        QueueFamilyIndices indices;

        // propriedades de fila no dispositivo
        uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

        std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

        // a struct VkQueueFamilyProperties possui detalhes dos tipos de operação que são suportadas
        // e o numero de filas que podemos fazer baseado nessa familia
        // quero achar uma delas que suporta o VK_QUEUE_GRAPHICS_BIT
        int i = 0;
        for (const auto& queueFamily : queueFamilies) {
            if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                indices.graphicsFamily = i;
            }

            if (indices.isComplete()) {
                break;
            }


            i++;
        }

        return indices;
    }

    /*
    // aqui inicia a criacao do dispositivo lógico
    void createLogicalDevice() {
        QueueFamilyIndices indices = findQueueFamilies(physicalDevice);


        // essa struct descreve o numero de filas que queremos, portanto
        // vou utilizar apenas a fila com capacidades graficas
        VkDeviceQueueCreateInfo queueCreateInfo{};

        // declarando que estou usando para criar uma fila de um dispositivo
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        // indicando que quero apenas a fila grafica
        queueCreateInfo.queueFamilyIndex = indices.graphicsFamily.value();
        queueCreateInfo.queueCount = 1;
    }
    */

    // faz uma limpeza na hora de sair/fechar
    void cleanup() {
        if (enableValidationLayers) {
            DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
        }

        vkDestroyInstance(instance, nullptr);

        glfwDestroyWindow(window);

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
