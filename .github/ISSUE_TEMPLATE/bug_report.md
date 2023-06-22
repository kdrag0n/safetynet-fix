---
name: Bug report
about: Report a bug or issue
title: ''
labels: bug
assignees: ''

---

<!--
Please fill out this entire template, including logs and device info.
MISSING INFORMATION MAY RESULT IN YOUR ISSUE BEING CLOSED.
-->

**Describe the bug**
A clear and concise description of what the bug is.

**To reproduce**
Steps to reproduce the behavior:
1. Go to '...'
2. Click on '....'
3. Scroll down to '....'
4. See error

**Expected behavior**
A clear and concise description of what you expected to happen.

**Screenshots of SafetyNet & Play Integrity API tests result**
Testing SafetyNet: https://play.google.com/store/apps/details?id=rikka.safetynetchecker
Testing Play Integrity API: https://play.google.com/store/apps/details?id=gr.nikolasspyr.integritycheck

**Device info**
Device model:
Android version:
ROM name/version:

**Logs**
Connect your phone to a computer and run `adb logcat > issue.log` then run SafetyNet & Play Integrity API tests. Stop log by `Ctrl + C` combination. Attach the log file to this issue.

**Additional context**
Add any other context about the problem here.

**Checklist**
- [ ] I confirm that the SELinux status is `Enforcing` on my device.
- [ ] All information is present
- [ ] Screenshots are attached
- [ ] Logs are attached
