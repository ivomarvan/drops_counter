from gtts import gTTS
import time
import os
#from googletrans import Translator
from translate import Translator

# root of project repository
from git_root_to_syspath import agr; PROJECT_ROOT = agr()

NOGIT_DATA = os.path.join(PROJECT_ROOT, 'nogit_data')
SOUND_DIR = os.path.join(NOGIT_DATA, 'sounds')

class SoundStorage:

    def __init__(
        self,
        work_dir: str = SOUND_DIR,
        phrases: [str, str] = [("ready", "připraven"), ("switch_off", "Prosím vypni napájení")],
        max_count: int = 120,
        languages: [str] = ["cs", "en", "de", "fr", "es"]
    ):
        self._work_dir = work_dir
        self._phrases = phrases
        self._max_count = max_count
        self._languages = languages
        for i in range(1, max_count + 1):
            self._phrases.append((str(i), str(i)))

    def _get_path(self, lang: str, name: str) -> str:
        return os.path.join(self._work_dir, lang, f"{name}.mp3")

    def _translate_word(self, word: str, target_lang: str) -> str:
        # Pokud je slovo číslo, neprovádějte překlad
        if word.isdigit():
            return word
        # Přeložit slovo
        translated = self._translator.translate(word)
        return translated

    def store_sound(self, lang, name, world):
        path = self._get_path(lang, name)
        if os.path.isfile(path):
            print(f'lang: {lang} name: {name} (world:{world}) already exists.')
            return
        os.makedirs(os.path.dirname(path), exist_ok=True)
        translated_word = self._translate_word(world, lang)
        tts = gTTS(text=translated_word, lang=lang)
        tts.save(path)
        print(f'lang: {lang} name: {name} (world:{world}) saved to {path}.')
        time.sleep(0.2)

    def store_all_sounds(self):
        for lang in self._languages:
            self._translator = Translator(from_lang='cs', to_lang=lang)
            for name, world in self._phrases:
                self.store_sound(lang, name, world)


if __name__ == '__main__':
    SoundStorage().store_all_sounds()
