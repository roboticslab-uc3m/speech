// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

#include "PiperSynthesizer.hpp"

#include <cctype> // std::tolower

#include <fstream>

#include <yarp/os/LogStream.h>

#include "LogComponent.hpp"
#include "json.hpp"

// -----------------------------------------------------------------------------

bool PiperSynthesizer::inspectModels(const std::filesystem::path & base)
{
    using json = nlohmann::json;

    try
    {
        for (const auto & entry : std::filesystem::directory_iterator(base))
        {
            if (entry.is_regular_file())
            {
                const auto & path = entry.path();
                const auto name  = path.stem(); // remove .json extension if present
                const auto model = base / name; // use full path

                if (path.extension() == ".json" && std::filesystem::exists(model))
                {
                    std::ifstream stream(path);
                    const auto config = json::parse(stream);

                    if (config.contains("language") && config.contains("dataset"))
                    {
                        const auto & language = config["language"];

                        model_entry entry = {
                            model.string(),
                            toLowerCase(language["code"].get<std::string>()),
                            toLowerCase(language["name_english"].get<std::string>()),
                            toLowerCase(config["dataset"].get<std::string>())
                        };

                        storage.emplace(name.string(), entry);

                        yCDebug(PIPER) << "Found model" << name.string()
                                       << "with language / code" << entry.language << "/" << entry.code
                                       << "and dataset" << entry.dataset;
                    }
                }
            }
        }
    }
    catch (const std::filesystem::filesystem_error & e)
    {
        yCError(PIPER) << "Filesystem error while inspecting models:" << e.what();
        return false;
    }
    catch (const json::exception & e)
    {
        yCError(PIPER) << "JSON exception while parsing models:" << e.what();
        return false;
    }

    return true;
}

// -----------------------------------------------------------------------------

void PiperSynthesizer::loadCurrentModel()
{
    if (synth)
    {
        ::piper_free(synth);
        synth = nullptr;
    }

    const std::filesystem::path model = current_model->path;
    auto config = std::filesystem::path(model.string() + ".json");

    yCDebug(PIPER) << "Loading model from:" << model.string();
    yCDebug(PIPER) << "Loading config from:" << config.string();

    synth = ::piper_create(model.string().c_str(), config.string().c_str(), eSpeakDataFullPath.c_str());
    options = ::piper_default_synthesize_options(synth);

    options.speaker_id = m_speakerId;
    options.length_scale = m_lengthScale;
    options.noise_scale = m_noiseScale;
    options.noise_w_scale = m_noiseW;
}

// -----------------------------------------------------------------------------

std::string PiperSynthesizer::toLowerCase(const std::string & str)
{
    std::string lowerStr = str;

    std::transform(lowerStr.begin(), lowerStr.end(), lowerStr.begin(),
                   [](unsigned char c) { return std::tolower(c); });

    return lowerStr;
}

// -----------------------------------------------------------------------------
