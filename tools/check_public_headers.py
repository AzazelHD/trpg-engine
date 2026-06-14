import subprocess
import tempfile
from pathlib import Path

# Configuration
ROOT_DIR = Path(__file__).resolve().parent.parent
PUBLIC_INCLUDE_DIR = ROOT_DIR / "include" / "engine"

VSDEVCMD = Path(
    r"C:\Program Files (x86)\Microsoft Visual Studio\18\BuildTools\Common7\Tools\VsDevCmd.bat"
)

# Find all public headers
header_files = list(PUBLIC_INCLUDE_DIR.rglob("*.h"))

print("Checking public headers for self-containment...")

success = []
fail = []

with tempfile.TemporaryDirectory(prefix="trpg_header_check_") as tmp_dir:
    work_dir = Path(tmp_dir)
    obj_dir = work_dir / "obj"
    obj_dir.mkdir(parents=True, exist_ok=True)

    for header in sorted(header_files):
        rel_header = header.relative_to(ROOT_DIR / "include")

        dummy_name = f"test_{header.stem}.cpp"
        dummy_path = work_dir / dummy_name
        obj_path = obj_dir / f"test_{header.stem}.obj"

        # Write dummy source in a temp folder to avoid polluting the repo.
        dummy_path.write_text(
            f'#include "{rel_header.as_posix()}"\n'
            'int main() { return 0; }\n',
            encoding="utf-8"
        )

        compile_cmd = (
            f'call "{VSDEVCMD}" -arch=x64 && '
            f'cl.exe /nologo '
            f'/I"{ROOT_DIR / "include"}" '
            f'/Fo"{obj_path}" '
            f'/c "{dummy_path}"'
        )

        print(f"Compiling {rel_header}... ", end="")

        try:
            subprocess.run(
                compile_cmd,
                capture_output=True,
                text=True,
                check=True,
                shell=True
            )

            print("OK")
            success.append(str(rel_header))

        except subprocess.CalledProcessError as e:
            print("FAIL")

            output = (e.stdout or "") + (e.stderr or "")

            fail.append({
                "header": str(rel_header),
                "output": output,
            })

print(f"\nSummary: {len(success)} OK, {len(fail)} FAIL")

if fail:
    print("\nFailed headers:\n")

    for item in fail:
        print(f"- {item['header']}")
        print(item["output"])
        print("-" * 80)