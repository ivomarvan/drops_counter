from gtts import gTTS
import time
import os
#from googletrans import Translator
from translate import Translator
from pprint import pprint

# root of project repository
from git_root_to_syspath import agr; PROJECT_ROOT = agr()

NOGIT_DATA = os.path.join(PROJECT_ROOT, 'nogit_data')
SOUND_DIR = os.path.join(NOGIT_DATA, 'SD_CARD')  # 'MP3'

class SoundStorage:

    def __init__(
        self,
        work_dir: str = SOUND_DIR,
        ready_phrase: (str, str) = ("ready", "připraven"),
        switch_off_phrase: (str, str) = ("switch_off", "Prosím vypni napájení"),
        max_count: int = 120,
        languages: [str] = ["cs", "en", "de", "fr", "es"]
    ):
        self._work_dir = work_dir
        self._max_count = max_count
        self._languages = languages
        self._phrases = [ready_phrase]
        for i in range(1, max_count + 1):
            self._phrases.append((str(i), str(i)))
        self._phrases.append(switch_off_phrase)

    def _get_path(self, lang_id, world_id) -> str:
        return os.path.join(self._work_dir, f'{lang_id:02}', f'{world_id:04}.mp3')

    def _translate_word(self, word: str) -> str:
        # Pokud je slovo číslo, neprovádějte překlad
        if word.isdigit():
            return word
        # Přeložit slovo
        translated = self._translator.translate(word)
        return translated

    def store_sound(self, lang_id, lang, name, world_id, world):
        path = self._get_path(lang_id, world_id)
        if os.path.isfile(path):
            print(f'lang: {lang} name: {name} (world:{world}) already exists.')
            return
        os.makedirs(os.path.dirname(path), exist_ok=True)
        translated_word = self._translate_word(world)
        tts = gTTS(text=translated_word, lang=lang)
        tts.save(path)
        print(f'lang: {lang} name: {name} (world:{world}) saved to {path}.')
        time.sleep(0.2)

    def store_all_sounds(self):
        languages = {}
        for lang_id, lang in enumerate(self._languages):
            # os.makedirs(os.path.join(self._work_dir, f'{lang_id:03}'), exist_ok=True)
            # with open(os.path.join(self._work_dir, f'{lang_id:03}', f'lang_{lang_id:03}_{lang}.txt'), 'w') as f:
            #     f.write(lang)
            self._translator = Translator(from_lang='cs', to_lang=lang)
            for world_id, (name, world) in enumerate(self._phrases):
                self.store_sound(lang_id, lang, name, world_id, world)
            languages[lang] = lang_id
        pprint(languages)

if __name__ == '__main__':
    SoundStorage().store_all_sounds()
